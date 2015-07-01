/** @file
    Implementacja węzła.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-25
 */

#include "node.h"
#include "set.h"
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
    Set *children;
};

/** @name Funkcje pomocnicze
  @{
  */

/*
 Porównuje dwa węzły
 */
static int compare_nodes(void *a, void *b)
{
    wchar_t _a = node_get_key(a);
    wchar_t _b = node_get_key(b);

    if (_a > _b) return 1;
    if (_a == _b) return 0;
    return -1;
}

/*
 Destrukcja węzła na potrzeby seta.
 */
static void free_node(void *node)
{
    node_done((Node*)node);
}

/**@}*/
/** @name Elementy interfejsu
  @{
  */

Node * node_new(const wchar_t character)
{
    Node *node = (Node *) malloc(sizeof(Node));
    if (!node)
    {
        fprintf(stderr, "Failed to allocate memory for node\n");
        exit(EXIT_FAILURE);
    }

    node->value = character;
    node->parent = NULL;
    node->children = set_new(compare_nodes, free_node);
    node->is_word = false;

    return node;
}

void node_done(Node *node)
{
    for (int i = 0; i < node_children_count(node); i++)
    {
        node_done(set_get_by_index(node->children, i));
    }

    set_done(node->children);
    free(node);
}

Node * node_add_child(Node *node, const wchar_t character)
{
    Node *child = node_new(character);
    child->parent = node;

    if (!set_insert(node->children, child))
    {
        node_done(child);
        return node_get_child(node, character);
    }

    return child;
}

Node * node_add_child_at_end(Node *node, const wchar_t character)
{
    Node *child = node_new(character);
    child->parent = node;

    set_insert_at_end(node->children, child);

    return child;
}

Node * node_get_child(const Node *node, const wchar_t character)
{
    Node *tmp = node_new(character);
    Node *ret = set_find(node->children, tmp);
    node_done(tmp);
    return ret;
}

Node * node_get_parent(const Node *node)
{
    return node->parent;
}

wchar_t node_get_key(const Node *node) {
    return node->value;
}

int node_remove_child(Node *node, const wchar_t character)
{
    Node *tmp = node_new(character);
    int ret = set_delete(node->children, tmp);
    node_done(tmp);
    return ret;
}

bool node_is_word(const Node *node)
{
    return node->is_word;
}

void node_set_is_word(Node *node, const bool is_word)
{
    node->is_word = is_word;
}

const int node_children_count(const Node *node)
{
    return set_size(node->children);
}

Node * node_get_child_by_index(const Node *node, const int index)
{
    return set_get_by_index(node->children, index);
}

bool node_has_word(const Node *node, const wchar_t *word)
{
    size_t word_length = wcslen(word);

    for (int i = 0; i < word_length; i++)
    {
        node = node_get_child(node, word[i]);
        if (node == NULL)
        {
            return false;
        }
    }

    return node_is_word(node);
}

void node_add_words_to_list(const Node *node, wchar_t *prefix,
                            const size_t depth, struct word_list *list)
{
    prefix[depth+1] = L'\0';

    for (size_t i = 0; i < node_children_count(node); i++)
    {
        Node *child = set_get_by_index(node->children, i);
        prefix[depth] = node_get_key(child);

        if (node_is_word(child)) word_list_add(list, prefix);

        node_add_words_to_list(child, prefix, depth + 1, list);
    }

    prefix[depth] = L'\0';
}

int node_save(const Node *node, IO *io)
{
    for (int i = 0; i < node_children_count(node); i++)
    {
        Node *child = set_get_by_index(node->children, i);

        if (io_printf(io, L"%lc", child->value) < 0) return -1;
        if (node_is_word(child) && io_printf(io, L"%lc", L'*') < 0) return -1;
        if (node_save(child, io) < 0) return -1;
        if (io_printf(io, L"%lc", L'^') < 0) return -1;
    }

    return 0;
}

/**@}*/
