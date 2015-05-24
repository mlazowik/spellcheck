/** @file
    Interfejs węzła.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-05-23
 */

#ifndef __NODE_H__
#define __NODE_H__

#include <wchar.h>
#include <stdbool.h>

/**
  Struktura przechowująca węzeł.
  */
typedef struct node Node;

/**
  Inicjalizacja węzła.
  Należy go zniszczyć za pomocą node_done()
  @param[in] character Znak nowego węzła.
  @return Nowy węzeł.
  */
Node * node_new(wchar_t character);

/**
  Destrukcja węzła.
  @param[in,out] node Węzeł.
  */
void node_done(Node *node);

/**
  Tworzy syna węzła dla określonego znaku.
  @param[in,out] node Węzeł.
  @param[in] character Znak dodawanego węzła.
  */
void node_add_child(Node *node, wchar_t character);

/**
  Zwraca syna węzła o określonym znaku.
  @param[in,out] node Węzeł.
  @param[in] character Znak szukanego węzła.
  @return Wskaźnik na syna lub NULL jeśli nie istnieje.
  */
Node * node_get_child(Node *node, wchar_t character);

/**
  Usuwa syna węzła o określonym znaku.
  @param[in,out] node Węzeł.
  @param[in] character Znak usuwangeo węzła.
  @return 1 jeśli się udało, 0 w p.p.
  */
int node_remove_child(Node *node, wchar_t character);

/**
  Sprawdza, czy istnieje syn o danym znaku.
  @param[in,out] node Węzeł.
  @param[in] character Sprawdzany znak.
  @return Wartość logiczna określająca czy znak istnieje.
  */
bool node_has_child(const Node *node, wchar_t character);

/**
  Sprawdza, czy w węźle kończy się słowo.
  @param[in,out] node Węzeł.
  @return Wartość logiczna określająca czy w węźle kończy się słowo.
  */
bool node_is_word(const Node *node);

/**
  Definiuje, czy w węzle kończy się słowo.
  @param[in,out] node Węzeł.
  @param[in] is_word Wartość logiczna określająca czy w węźle kończy się słowo
  */
void node_set_is_word(Node *node, bool is_word);

#endif /* __NODE_H__ */
