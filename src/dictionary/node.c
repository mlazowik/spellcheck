/** @file
    Implementacja węzła.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-05-24
 */

#include "node.h"
#include "map.h"
#include <stdlib.h>

/**
  Struktura przechowująca węzeł.
  */
struct node
{
    /// Wartość w węźle.
    wchar_t value;
    /// Czy w węźle kończy się słowo.
    bool is_word;

    /// Rodzic węzła.
    Node *parent;
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
    node->parent = NULL;
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
    child->parent = node;
    map_insert(node->children, character, child);
}

Node * node_get_child(const Node *node, wchar_t character)
{
    return map_find(node->children, character);
}

Node * node_get_parent(const Node *node)
{
    return node->parent;
}

wchar_t node_get_key(const Node *node) {
    return node->value;
}

int node_remove_child(Node *node, wchar_t character)
{
    return map_delete(node->children, character);
}

bool node_has_child(const Node *node, wchar_t character)
{
    return (map_find(node->children, character) != NULL);
}

bool node_is_word(const Node *node)
{
    return node->is_word;
}

void node_set_is_word(Node *node, bool is_word)
{
    node->is_word = is_word;
}

int node_children_count(const Node *node)
{
    return map_size(node->children);
}

/**@}*/
