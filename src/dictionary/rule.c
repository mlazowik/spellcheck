/** @file
    Implementacja dynamicznego zbioru.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-21
 */

#include "rule.h"
#include "vector.h"
#include "node.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

/**
  Struktura przechowująca regułę.
  */
struct rule
{
    wchar_t *left;          ///< Lewa strona reguły.
    size_t left_len;        ///< Długość lewej strony.
    wchar_t *right;         ///< Prawa strona reguły.
    size_t right_len;       ///< Długość prawej strony.
    int cost;               ///< Koszt reguły.
    enum rule_flag flag;    ///< Użyta flaga bądź jej brak.
    wchar_t vars[10];       ///< Wartości zmiennych.
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

/*
 Łaszywe usuwanie.
 */
static void fake_free(void *state)
{
    return;
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

/*
 Zlicza liczbę (unikalnych) zmiennych występujących tylko
 po prawej stronie reguły.
 */
static int vars_only_in_right(Rule *rule)
{
    bool var_present_in_left[10];
    bool var_present_in_right[10];

    for (size_t i = 0; i < 10; i++)
    {
        var_present_in_left[i] = false;
        var_present_in_right[i] = false;
    }

    for (size_t i = 0; i < rule->left_len; i++)
    {
        if (is_decimal(rule->left[i]))
        {
            var_present_in_left[decimal_to_int(rule->left[i])] = true;
        }
    }

    for (size_t i = 0; i < rule->right_len; i++)
    {
        if (is_decimal(rule->right[i]))
        {
            var_present_in_right[decimal_to_int(rule->right[i])] = true;
        }
    }

    int only_in_right = 0;
    for (size_t i = 0; i < 10; i++)
    {
        if (!var_present_in_left[i]
            && var_present_in_right[i])
        {
            only_in_right++;
        }
    }

    return only_in_right;
}

/**
  Destrukcja znaku.
  @param el Wartość do usunięcia
  */
static void free_char(void *el)
{
    free(el);
}

/**
  Tworzenie znaku
  @param a Wartość którą ma przyjąć.
  */
static wchar_t * char_new(wchar_t a)
{
    int *ret = (wchar_t*) malloc(sizeof(wchar_t));
    *ret = a;
    return ret;
}

static wchar_t * char_vector_to_string(Vector *chars)
{
    wchar_t *str = malloc(sizeof(wchar_t) * (vector_size(chars) + 1));

    for (size_t i = 0; i < vector_size(chars); i++)
    {
        str[i] = *(wchar_t*)vector_get_by_index(chars, i);
    }

    str[vector_size(chars)] = L'\0';

    return str;
}

static wchar_t * read_rule_str(IO *io, wchar_t separator)
{
    wint_t c;
    wchar_t *ret;

    Vector *chars = vector_new(free_char);

    while ((c = io_get_next(io)) != separator && c != WEOF)
    {
        vector_push_back(chars, char_new(c));
    }

    if (c == WEOF)
    {
        vector_clear(chars);
        vector_done(chars);
        return NULL;
    }

    ret = char_vector_to_string(chars);

    vector_clear(chars);
    vector_done(chars);

    return ret;
}

static const int read_rule_int(IO *io, wchar_t separator)
{
    long int ret;
    wchar_t *last, *str;
    bool is_zero, parsed_whole, out_of_range, parsing_failed, is_empty;

    if (!(str = read_rule_str(io, separator))) return -1;
    ret = wcstol(str, &last, 10);

    is_empty = (wcslen(str) == 0);
    is_zero = (str[0] == L'0' && str[1] == L'\0');
    parsing_failed = (ret == 0 && !is_zero);
    parsed_whole = (*last == '\0');
    out_of_range = (ret < 0 || ret > INT_MAX || ret == LONG_MAX);

    free(str);

    if (!parsed_whole || out_of_range || parsing_failed || is_empty) return -1;

    return ret;
}

static bool set_vars(Rule *rule, const wchar_t *word)
{
    for (size_t i = 0; i < 10; i++) rule->vars[i] = L'\0';

    for (size_t i = 0; i < rule->left_len; i++)
    {
        if (is_decimal(rule->left[i]))
        {
            int var = decimal_to_int(rule->left[i]);
            if (rule->vars[var] == L'\0') rule->vars[var] = word[i];
            else if (rule->vars[var] != word[i]) return false;
        }
        else
        {
            if (rule->left[i] != word[i]) return false;
        }
    }

    return true;
}

static Vector * get_next_nodes(Rule *rule, Node *node)
{
    wchar_t right[rule->right_len];
    int free_var = -1;
    Vector *nodes = vector_new(fake_free);

    for (size_t i = 0; i < rule->right_len; i++)
    {
        if (is_decimal(rule->right[i]))
        {
            int var = decimal_to_int(rule->right[i]);
            if (rule->vars[var] == L'\0')
            {
                free_var = var;
                right[i] = rule->right[i];
            }
            else
            {
                right[i] = rule->vars[var];
            }
        }
        else
        {
            right[i] = rule->right[i];
        }
    }

    int j = 0;
    while (j < rule->right_len && !is_decimal(right[j]))
    {
        node = node_get_child(node, right[j]);
        if (node == NULL) return nodes;
        j++;
    }

    if (free_var == -1)
    {
        vector_push_back(nodes, node);
        return nodes;
    }

    for (size_t i = 0; i < node_children_count(node); i++)
    {
        Node *tmp = node_get_child_by_index(node, i);
        rule->vars[free_var] = node_get_key(tmp);
        tmp = node;

        int k = j;
        while (k < rule->right_len && tmp != NULL)
        {
            if (is_decimal(right[k]))
            {
                tmp = node_get_child(tmp, rule->vars[free_var]);
            }
            else
            {
                tmp = node_get_child(tmp, right[k]);
            }
            k++;
        }

        if (tmp != NULL) vector_push_back(nodes, tmp);
    }

    return nodes;
}

/**@}*/
/** @name Elementy interfejsu
  @{
  */

Rule * rule_new(const wchar_t *left, const wchar_t *right,
                int cost, enum rule_flag flag)
{
    Rule *rule = (Rule *) emalloc(sizeof(Rule));

    rule->left_len = wcslen(left);
    rule->right_len = wcslen(right);

    rule->left = emalloc(sizeof(wchar_t) * (rule->left_len + 1));
    rule->right = emalloc(sizeof(wchar_t) * (rule->right_len +1 ));

    wcscpy(rule->left, left);
    wcscpy(rule->right, right);
    rule->cost = cost;
    rule->flag = flag;

    return rule;
}

void rule_done(Rule *rule) {
    free(rule->left);
    free(rule->right);
    free(rule);
}

int rule_get_cost(Rule *rule)
{
    return rule->cost;
}

bool rule_matches_prefix(Rule *rule, bool is_start, const wchar_t *word)
{
    int len = wcslen(word);

    if (len < rule->left_len) return false;

    return set_vars(rule, word);
}

Vector * rule_apply(Rule *rule, State *state, Node *root)
{
    Vector *states = vector_new(fake_free);

    if (rule->flag == RULE_BEGIN && (!state->prev && state->node != root))
    {
        return states;
    }

    if (rule->flag == RULE_SPLIT && (!state->prev && state->node == root))
    {
        return states;
    }

    if (rule->flag == RULE_SPLIT && state->prev != NULL) return states;

    set_vars(rule, state->sufix);

    Vector *nodes = get_next_nodes(rule, state->node);
    for (size_t i = 0; i < vector_size(nodes); i++)
    {
        Node *node = vector_get_by_index(nodes, i);
        State *new_state = state_new(node, state->prev,
                                     state->sufix + rule->left_len,
                                     state->cost + rule->cost,
                                     state->sufix_len - rule->left_len,
                                     (rule->flag != RULE_END));

        if (rule->flag == RULE_SPLIT)
        {
            new_state->prev = new_state->node;
            new_state->node = root;

            if (node_is_word(new_state->prev))
            {
                vector_push_back(states, new_state);
            }
            else
            {
                state_done(new_state);
            }
        }
        else
        {
            vector_push_back(states, new_state);
        }
    }

    vector_done(nodes);

    return states;
}

bool rule_is_legal(Rule *rule)
{
    if (vars_only_in_right(rule) > 1)
    {
        return false;
    }

    if (rule->left_len == 0 && rule->right_len == 0 && rule->flag != RULE_SPLIT)
    {
        return false;
    }

    return true;
}

int rule_save(const Rule *rule, IO *io)
{
    if (io_printf(io, L"%ls*", rule->left) < 0) return -1;
    if (io_printf(io, L"%ls*", rule->right) < 0) return -1;
    if (io_printf(io, L"%d*", rule->cost) < 0) return -1;
    if (io_printf(io, L"%d\n", rule->flag) < 0) return -1;

    return 0;
}

Rule * rule_load(IO *io)
{
    Rule *rule = NULL;

    wchar_t *left = NULL, *right = NULL;
    int cost = 0;
    enum rule_flag flag = 0;

    if (!(left = read_rule_str(io, L'*'))) goto cleanup;
    if (!(right = read_rule_str(io, L'*'))) goto cleanup;
    if ((cost = read_rule_int(io, L'*')) == -1) goto cleanup;
    if ((flag = read_rule_int(io, L'\n')) == -1) goto cleanup;

    rule = rule_new(left, right, cost, flag);

cleanup:
    free(left);
    free(right);

    return rule;
}

/**@}*/
