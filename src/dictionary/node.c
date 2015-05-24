/** @file
    Implementacja węzła.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-05-23
 */

#include "node.h"
#include "map.h"
#include "stdlib.h"

/**
  Struktura przechowująca węzeł.
  */
struct node
{
    /// Wartość w węźle.
    wchar_t value;
    /// Czy w węźle kończy się słowo.
    bool is_word;

    /// Dzieci węzła.
    Map *children;
};

/** @name Elementy interfejsu
  @{
  */

Node * node_new(wchar_t character)
{
    Node *node = (Node *) malloc(sizeof(Node));

    node->value = character;
    node->children = map_new();

    return node;
}

void node_done(Node *node)
{
    map_done(node->children);
    free(node);
}

void node_add_child(Node *node, wchar_t character)
{
    Node *child = node_new(character);
    map_insert(node->children, character, child);
}

Node * node_get_child(Node *node, wchar_t character)
{
    return map_find(node->children, character);
}

int node_remove_child(Node *node, wchar_t character)
{
    return map_delete(node->children, character);
}

bool node_has_child(const Node *node, wchar_t character)
{
    return (map_find(node->children, character) != NULL);
}

bool node_id_word(const Node *node)
{
    return node->is_word;
}

void node_set_is_word(Node *node, bool is_word)
{
    node->is_word = is_word;
}

/**@}*/
