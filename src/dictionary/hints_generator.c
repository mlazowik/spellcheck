/** @file
    Implementacja gena podpowiedzi.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-22
 */

#include "hints_generator.h"
#include "state.h"
#include "node.h"
#include "set.h"
#include "vector.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>

/**
  Struktura przechowująca gen podpowiedzi.
  */
struct hints_generator
{
    /// Maksymalny koszt podpowiedzi.
    int max_cost;
    /// Największy koszt wśród reguł.
    int max_rule_cost;
    /// Korzeń drzewa trie.
    Node *root;
    /// Reguły tworzenia podpowiedzi.
    Vector *rules;
    /// Reguły wg. kosztu i sufiksu do którego pasują.
    Vector ***word_rules;
    /// Stany.
    Vector *states;
    /// Stany będące unikalnymi podpowiedziami
    Set *hint_states;
};

/** @name Funkcje pomocnicze
  @{
  */

/*
 malloc opakowany w obsługę błedu
 */
static void * emalloc(size_t el_size)
{
    void *ret = malloc(el_size);
    if (!ret)
    {
        fprintf(stderr, "Failed to allocate memory for rule\n");
        exit(EXIT_FAILURE);
    }

    return ret;
}

static int compare_state(const void *_a, const void *_b)
{
    State *a = (State*) _a;
    State *b = (State*) _b;

    if ((uintptr_t)a->node < (uintptr_t)b->node) return 1;
    if ((uintptr_t)a->node > (uintptr_t)b->node) return -1;

    if ((uintptr_t)a->sufix < (uintptr_t)b->sufix) return 1;
    if ((uintptr_t)a->sufix > (uintptr_t)b->sufix) return -1;

    if (!a->prev && b->prev) return 1;
    if (a->prev && !b->prev) return -1;

    if ((uintptr_t)a->prev < (uintptr_t)b->prev) return 1;
    if ((uintptr_t)a->prev > (uintptr_t)b->prev) return -1;

    if (a->expandable && !b->expandable) return 1;
    if (!a->expandable && b->expandable) return -1;

    return 0;
}

static int compare_hint_states(void *_a, void *_b)
{
    State *a = (State*) _a;
    State *b = (State*) _b;

    if ((uintptr_t)a->node < (uintptr_t)b->node) return 1;
    if ((uintptr_t)a->node > (uintptr_t)b->node) return -1;

    if (!a->prev && b->prev) return 1;
    if (a->prev && !b->prev) return -1;

    if ((uintptr_t)a->prev < (uintptr_t)b->prev) return 1;
    if ((uintptr_t)a->prev > (uintptr_t)b->prev) return -1;

    return 0;
}

static int compare_hint_strings(const void *_a, const void *_b)
{
    State *a = *(State**) _a;
    State *b = *(State**) _b;

    if (a->cost < b->cost) return -1;
    if (a->cost > b->cost) return 1;

    return wcscoll(a->string, b->string);
}

/*
 Usuwanie stanu na potrzeby wektora.
 */
static void free_state(void *state)
{
    state_done((State*)state);
}

/*
 Usuwanie reguły na potrzeby wektora.
 */
static void free_rule(void *rule)
{
    rule_done(rule);
}

static void init_word_rules(Hints_Generator *gen, int word_len)
{
    gen->word_rules = emalloc(sizeof(Vector**) * (gen->max_rule_cost + 1));

    for (size_t i = 0; i <= gen->max_rule_cost; i++)
    {
        gen->word_rules[i] = emalloc(sizeof(Vector*) * (word_len + 1));
        for (size_t j = 0; j <= word_len; j++)
        {
            gen->word_rules[i][j] = vector_new(free_rule);
        }
    }
}

static void match_rules_to_word(Hints_Generator *gen, const wchar_t *word)
{
    size_t len = wcslen(word);

    for (size_t i = 0; i < vector_size(gen->rules); i++)
    {
        Rule *rule = vector_get_by_index(gen->rules, i);
        for (size_t j = 0; j <= len; j++)
        {
            if (rule_matches_prefix(rule, (j == 0), word+j))
            {
                int cost = rule_get_cost(rule);
                vector_push_back(gen->word_rules[cost][len-j], rule);
            }
        }
    }
}

static void free_word_rules(Hints_Generator *gen, int word_len)
{
    for (size_t i = 0; i <= gen->max_rule_cost; i++)
    {
        for (size_t j = 0; j <= word_len; j++)
        {
            vector_done(gen->word_rules[i][j]);
        }
        free(gen->word_rules[i]);
    }
    free(gen->word_rules);
}

/*
 Dodaje stan lub podmienia, jeśli lepszy koszt.
 */
static void add_state(Hints_Generator *gen, State *state)
{
    vector_push_back(gen->states, state);

    if (node_is_word(state->node) && state->sufix[0] == L'\0')
    {
        set_insert(gen->hint_states, state);
    }
}

/*
 Dodaje stan i jego pochodne.
 */
static void add_extended_states(Hints_Generator *gen, State *state)
{
    add_state(gen, state);

    if (!state->expandable) return;

    while (state->sufix_len > 0
           && node_get_child(state->node, state->sufix[0]) != NULL)
    {
        Node *child = node_get_child(state->node, state->sufix[0]);
        state = state_new(child, state->prev, state->sufix+1, state->cost,
                          state->sufix_len-1, state->expandable);
        add_state(gen, state);
    }
}

static void add_states(Hints_Generator *gen, int cost)
{
    size_t n_states = vector_size(gen->states);

    for (size_t i = 0; i < n_states; i++)
    {
        State *state = vector_get_by_index(gen->states, i);
        if (cost - state->cost <= gen->max_rule_cost)
        {
            Vector *rules = gen->word_rules[cost - state->cost][state->sufix_len];
            for (size_t j = 0; j < vector_size(rules); j++)
            {
                Rule *rule = vector_get_by_index(rules, j);
                Vector *new_states = rule_apply(rule, state, gen->root);
                for (size_t k = 0; k < vector_size(new_states); k++)
                {
                    State *new_state = vector_get_by_index(new_states, k);
                    add_extended_states(gen, new_state);
                }
                vector_done(new_states);
            }
        }
    }
}

static void remove_duplicates(Hints_Generator *gen)
{
    Vector *deduplicated = vector_new(free_state);

    vector_sort(gen->states, compare_state);

    State *cur, *prev = NULL;
    for (size_t i = 0; i < vector_size(gen->states); i++)
    {
        cur = vector_get_by_index(gen->states, i);
        if (prev && compare_state(cur, prev) == 0)
        {
            state_done(cur);
        }
        else
        {
            vector_push_back(deduplicated, cur);
            prev = cur;
        }
    }

    vector_done(gen->states);

    gen->states = deduplicated;
}

/*
 Zlicza już znalezione podpowiedzi.
 */
static int count_hints(Hints_Generator *gen)
{
    return set_size(gen->hint_states);
}

static void get_hints(Hints_Generator *gen, struct word_list *list)
{
    Vector *all_hints = vector_new(free_state);

    for (size_t i = 0; i < vector_size(gen->states); i++)
    {
        State *state = vector_get_by_index(gen->states, i);
        if (node_is_word(state->node) && state->sufix_len == 0)
        {
            state->string = state_to_string(state);
            vector_push_back(all_hints, state);
        }
    }

    vector_sort(all_hints, compare_hint_strings);

    for (size_t i = 0; i < vector_size(all_hints); i++)
    {
        State *state = vector_get_by_index(all_hints, i);

        if (word_list_size(list) < DICTIONARY_MAX_HINTS)
        {
            bool different = true;
            if (word_list_size(list) > 0)
            {
                const wchar_t *prev = word_list_get(list)[word_list_size(list) - 1];
                if (wcscoll(state->string, prev) == 0) different = false;
            }
            if (different)
            {
                word_list_add(list, state->string);
            }
        }

        free(state->string);
    }

    vector_done(all_hints);
}

/*
 Sprawdza czy znak jest cyfrą dzisiętną.
 Potrzebne, bo iswdigit zależnie od locale może uznawać
 inne znaki niż dziesiętne.
 */
static bool is_decimal(wchar_t wc)
{
    wchar_t digits[] = {
        L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9'
    };

    for (size_t i = 0; i < 10; i++) if (wc == digits[i]) return true;

    return false;
}

/*
 Konwertuje znak na liczbę
 */
static int decimal_to_int(wchar_t wc)
{
    return wc - L'0';
}

/**@}*/
/** @name Elementy interfejsu
  @{
  */

Hints_Generator * hints_generator_new()
{
    Hints_Generator *gen;

    gen = (Hints_Generator*) emalloc(sizeof(Hints_Generator));
    gen->max_cost = 0;
    gen->max_rule_cost = 0;
    gen->root = NULL;
    gen->rules = vector_new(free_rule);
    gen->states = NULL;

    return gen;
}

void hints_generator_done(Hints_Generator *gen)
{
    vector_clear(gen->rules);
    vector_done(gen->rules);
    free(gen);
}

void hints_generator_set_root(Hints_Generator *gen, Node *root)
{
    gen->root = root;
}

void hints_generator_hints(Hints_Generator *gen, const wchar_t* word,
                           struct word_list *list)
{
    int len = wcslen(word);
    init_word_rules(gen, len);
    match_rules_to_word(gen, word);

    gen->states = vector_new(free_state);
    gen->hint_states = set_new(compare_hint_states, free_state);

    add_extended_states(gen, state_new(gen->root, NULL, word, 0, len, true));

    int k = 1;
    while (count_hints(gen) < DICTIONARY_MAX_HINTS
           && k <= gen->max_cost)
    {
        add_states(gen, k);
        remove_duplicates(gen);
        k++;
    }

    get_hints(gen, list);

    set_done(gen->hint_states);
    vector_clear(gen->states);
    vector_done(gen->states);

    free_word_rules(gen, len);
}

int hints_generator_max_cost(Hints_Generator *gen, int new_cost)
{
    int old_cost = gen->max_cost;
    gen->max_cost = new_cost;
    return old_cost;
}

void hints_generator_rule_clear(Hints_Generator *gen)
{
    vector_clear(gen->rules);

    gen->max_rule_cost = 0;
}

void hints_generator_rule_add(Hints_Generator *gen, Rule *rule)
{
    vector_push_back(gen->rules, rule);

    if (rule_get_cost(rule) > gen->max_rule_cost)
    {
        gen->max_rule_cost = rule_get_cost(rule);
    }
}

int hints_generator_save(const Hints_Generator *gen, IO *io)
{
    if (io_printf(io, L"%d\n", gen->max_cost) < 0) return -1;
    for (size_t i = 0; i < vector_size(gen->rules); i++)
    {
        if (rule_save(vector_get_by_index(gen->rules, i), io) < 0)
        {
            return -1;
        }
    }

    return 0;
}

Hints_Generator * hints_generator_load(IO *io)
{
    int cost = -1;
    wint_t c;
    while ((c = io_get_next(io)) != L'\n' && c != WEOF)
    {
        if (!is_decimal(c)) return NULL;
        if (cost == -1) cost = 0;
        cost *= 10;
        cost += decimal_to_int(c);
    }
    if (cost == -1) return NULL;

    Hints_Generator *gen = hints_generator_new();
    hints_generator_max_cost(gen, cost);

    while (io_peek_next(io) != WEOF)
    {
        Rule *rule = rule_load(io);
        if (!rule)
        {
            hints_generator_done(gen);
            return NULL;
        }
        hints_generator_rule_add(gen, rule);
    }

    return gen;
}

/**@}*/
