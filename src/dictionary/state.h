/** @file
    Interfejs stanu modyfikacji słowa.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-26
 */

#ifndef __STATE_H__
#define __STATE_H__

#include <stdbool.h>
#include "node.h"

/**
  Struktura przechowująca stan.
  */
typedef struct state State;

/**
  Struktura przechowująca stan.
  */
struct state
{
    /// Nieprzetworzona część słowa.
    const wchar_t *sufix;
    /// Napis przechowywany przez stan.
    wchar_t *string;
    /// Węzeł.
    Node *node;
    /// Węzeł pierwszego słowa (jeśli był rozdział).
    Node *prev;
    /// Koszt stanu.
    int cost;
    /// Długość nieprzetworzonej części słowa.
    int sufix_len;
    /// Czy stan można rozszerzać.
    bool expandable;
};

/**
  Tworzy stan.
  Należy go zniszczyć za pomocą state_done.

  @param node Węzeł.
  @param prev Węzeł pierwszego słowa.
  @param sufix Nieprzetworzona część słowa.
  @param cost Koszt.
  @param sufix_len Długość nieprzetworzonej części słowa
  @param expandable Czy jest rozszerzalny.
  @return Nowy stan.
  */
State * state_new(Node *node, Node *prev, const wchar_t *sufix, int cost,
                         int sufix_len, bool expandable);

/**
  Niszczy stan.
  @param state Stan.
  */
void state_done(State *state);

/**
  Zwraca napis przechowywany przez stan.

  @param state Stan.
  @return Napis stanu.
  */
wchar_t * state_to_string(State *state);

#endif /* STATE_GENERATOR */