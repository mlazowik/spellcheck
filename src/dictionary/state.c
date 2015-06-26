/** @file
    Implementacja gena podpowiedzi.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-26
 */

#include "state.h"
#include "node.h"
#include <stdlib.h>
#include <string.h>


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

static int get_state_string_len(State *state)
{
    int len = 0;

    Node *node = state->node;
    while (node_get_key(node) != L'\0')
    {
        len++;
        node = node_get_parent(node);
    }

    if (state->prev != NULL)
    {
        len++;
        node = state->prev;
        while (node_get_key(node) != L'\0')
        {
            len++;
            node = node_get_parent(node);
        }
    }

    return len;
}

/**@}*/
/** @name Elementy interfejsu
  @{
  */

State * state_new(Node *node, Node *prev, const wchar_t *sufix, int cost,
                         int sufix_len, bool expandable)
{
    State *state = (State*) emalloc(sizeof(State));

    state->node = node;
    state->prev = prev;
    state->sufix = sufix;
    state->string = NULL;
    state->cost = cost;
    state->sufix_len = sufix_len;
    state->expandable = expandable;

    return state;
}

void state_done(State *state)
{
    free(state);
}

wchar_t * state_to_string(State *state)
{
    int len = get_state_string_len(state);
    wchar_t *string = emalloc(sizeof(wchar_t) * (len+1));
    string[len--] = L'\0';

    Node *node = state->node;
    while (node_get_key(node) != L'\0')
    {
        string[len--] = node_get_key(node);
        node = node_get_parent(node);
    }

    if (state->prev != NULL)
    {
        string[len--] = L' ';
        node = state->prev;
        while (node_get_key(node) != L'\0')
        {
            string[len--] = node_get_key(node);
            node = node_get_parent(node);
        }
    }

    return string;
}

/**@}*/
