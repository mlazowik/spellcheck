/** @file
    Interfejs generatora podpowiedzi.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-22
 */

#ifndef __STATE_H__
#define __STATE_H__

#include <stdbool.h>
#include "node.h"

typedef struct state State;

struct state
{
    const wchar_t *sufix;
    wchar_t *string;
    Node *node, *prev;
    int cost, sufix_len;
    bool expandable;
};

State * state_new(Node *node, Node *prev, const wchar_t *sufix, int cost,
                         int prefix_len, bool expandable);

void state_done(State *state);

wchar_t * state_to_string(State *state);

void print_state(State *state);

#endif /* STATE_GENERATOR */