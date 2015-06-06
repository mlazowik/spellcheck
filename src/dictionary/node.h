/** @file
    Interfejs węzła.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-05
 */

#ifndef __NODE_H__
#define __NODE_H__

#include <wchar.h>
#include <stdbool.h>
#include <stdio.h>
#include "word_list.h"
#include "trie.h"

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
Node * node_new(const wchar_t character);

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
void node_add_child(Node *node, const wchar_t character);

/**
  Zwraca syna węzła o określonym znaku.
  @param[in] node Węzeł.
  @param[in] character Znak szukanego węzła.
  @return Wskaźnik na syna lub NULL jeśli nie istnieje.
  */
Node * node_get_child(const Node *node, const wchar_t character);

/**
  Zwraca syna węzła o określonym znaku.
  @param[in] node Węzeł.
  @return Wskaźnik na ojca lub NULL jeśli nie istnieje.
  */
Node * node_get_parent(const Node *node);

/**
  Zwraca klucz danego węzła.
  @param[in] node Węzeł.
  @return Klucz węzła.
  */
wchar_t node_get_key(const Node *node);

/**
  Usuwa syna węzła o określonym znaku.
  @param[in,out] node Węzeł.
  @param[in] character Znak usuwangeo węzła.
  @return 1 jeśli się udało, 0 w p.p.
  */
int node_remove_child(Node *node, const wchar_t character);

/**
  Sprawdza, czy w węźle kończy się słowo.
  @param[in] node Węzeł.
  @return Wartość logiczna określająca czy w węźle kończy się słowo.
  */
bool node_is_word(const Node *node);

/**
  Definiuje, czy w węzle kończy się słowo.
  @param[in,out] node Węzeł.
  @param[in] is_word Wartość logiczna określająca czy w węźle kończy się słowo
  */
void node_set_is_word(Node *node, const bool is_word);

/**
  Zwraca liczbę dzieci węzła.
  @param[in] node Węzeł.
  @return Liczba dzieci węzła.
  */
const int node_children_count(const Node *node);

/**
  Sprawdza, czy podddrzewo zawiera dane słowo.
  @param[in] node Węzeł.
  @param[in] word Sprawdzane słowo.
  @return Wartość logiczna określająca czy słowo istnieje.
  */
bool node_has_word(const Node *node, const wchar_t *word);

/**
  Dodaje podpowiedzi dla danego słowa i węzła do drzewa podpowiedzi.
  @param[in] node Węzeł.
  @param[in] word Sprawdzane słowo.
  @param[in,out] hints Drzewo podpowiedzi.
  @return Wartość logiczna określająca czy słowo istnieje.
 */
void node_get_hints(const Node *node, const wchar_t *word, Trie *hints);

/**
  Dodaje słowa kończące się w dzieciach węzła do listy słów.
  @param[in] node Węzeł.
  @param[in] prefix Prefiks dla wszystkich słów.
  @param[in] depth Głębokość w drzewie.
  @param[in,out] list Lista do której są dodawane słowa.
  */
void node_add_words_to_list(const Node *node, wchar_t *prefix,
                            const size_t depth, struct word_list *list);

/**
  Zapisuje poddrzewo danego węzła.
  @param[in] node Węzeł.
  @param[in,out] io We/wy.
  @return <0 jeśli operacja się nie powiedzie, 0 w p.p.
  */
int node_save(const Node *node, IO *io);

#endif /* __NODE_H__ */
