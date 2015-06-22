/** @file
    Implementacja generatora podpowiedzi.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-22
 */

#include "hints_generator.h"
#include "node.h"
#include "set.h"
#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

/**
  Struktura przechowująca generator podpowiedzi.
  */
struct hints_generator
{
    /// Maksymalny koszt podpowiedzi.
    int max_cost;
    /// Reguły tworzenia podpowiedzi.
    Vector *rules_by_cost;
    /// Stany.
    Set *states;
};

struct state
{
    wchar_t *sufix;
    Node *node, *prev;
    int cost;
};

typedef struct state State;

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

static State * state_new(wchar_t *sufix, Node *node)
{
    State *state = (State*) emalloc(sizeof(State));

    state->node = node;
    state->prev = NULL;
    state->sufix = sufix;
    state->cost = 0;

    return state;
}

/*
 Usuwanie reguły na potrzeby wektora.
 */
static void free_rule(void *rule)
{
    rule_done(rule);
}

/*
 Usuwanie wektora reguł na potrzeby wektora.
 */
static void free_vector(void *vector)
{
    vector_clear(vector);
    vector_done(vector);
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
    Hints_Generator *generator;

    generator = (Hints_Generator*) emalloc(sizeof(Hints_Generator));
    generator->max_cost = 0;
    generator->rules_by_cost = vector_new(free_vector);
    generator->states = NULL;

    return generator;
}

void hints_generator_done(Hints_Generator *generator)
{
    vector_clear(generator->rules_by_cost);
    vector_done(generator->rules_by_cost);
    free(generator);
}

void hints_generator_hints(Hints_Generator *generator, Node *root,
                           const wchar_t* word, struct word_list *list)
{
}

int hints_generator_max_cost(Hints_Generator *generator, int new_cost)
{
    int old_cost = generator->max_cost;
    generator->max_cost = new_cost;
    return old_cost;
}

void hints_generator_rule_clear(Hints_Generator *generator)
{
    vector_clear(generator->rules_by_cost);
}

void hints_generator_rule_add(Hints_Generator *generator, Rule *rule)
{
    int cost = rule_get_cost(rule);

    while (vector_size(generator->rules_by_cost) < cost + 1)
    {
        vector_push_back(generator->rules_by_cost, vector_new(free_rule));
    }

    Vector *cost_vector = vector_get_by_index(generator->rules_by_cost, cost);

    vector_push_back(cost_vector, rule);
}

int hints_generator_save(const Hints_Generator *generator, IO *io)
{
    if (io_printf(io, L"%d\n", generator->max_cost) < 0) return -1;
    for (size_t i = 0; i < vector_size(generator->rules_by_cost); i++)
    {
        Vector *cost_vector = vector_get_by_index(generator->rules_by_cost, i);
        for (size_t j = 0; j < vector_size(cost_vector); j++)
        {
            if (rule_save(vector_get_by_index(cost_vector, j), io) < 0)
            {
                return -1;
            }
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

    Hints_Generator *generator = hints_generator_new();
    hints_generator_max_cost(generator, cost);

    while (io_peek_next(io) != WEOF)
    {
        fprintf(stderr, "%lc\n", io_peek_next(io));
        Rule *rule = rule_load(io);
        if (!rule)
        {
            hints_generator_done(generator);
            return NULL;
        }
        hints_generator_rule_add(generator, rule);
    }

    return generator;
}

/**@}*/
