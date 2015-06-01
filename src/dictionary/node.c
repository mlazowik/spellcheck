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

/** @name Funkcje pomocnicze
 @{
 */

/*
 Dodaje do listy istniejące słowa, które można uzyskać przez dodanie litery
 przed pozycją pos.
 */
static void get_hints_add(const Node *node, wchar_t *add,
                          size_t pos, struct word_list *list)
{
    for (size_t j = 0; j < node_children_count(node); j++)
    {
        Node *child = map_get_by_index(node->children, j);
        add[pos] = node_get_key(child);

        if (node_has_word(child, add + pos + 1))
        {
            word_list_add(list, add);
        }
    }
}

/*
 Dodaje do listy istniejące słowa, które można uzyskać przez zamianę litery
 na pozycji pos.
 */
static void get_hints_replace(const Node *node, wchar_t *replace,
                          size_t pos, struct word_list *list)
{
    for (size_t j = 0; j < node_children_count(node); j++)
    {
        Node *child = map_get_by_index(node->children, j);
        replace[pos] = node_get_key(child);

        if (node_has_word(child, replace + pos + 1))
        {
            word_list_add(list, replace);
        }
    }
}

/*
 Dodaje do listy istniejące słowa, które można uzyskać przez usunięcie litery
 z pozycji pos.
 */
static void get_hints_remove(const Node *node, wchar_t *remove,
                          size_t pos, struct word_list *list)
{
    if (node_has_word(node, remove + pos))
    {
        word_list_add(list, remove);
    }
}

/**@}*/
/** @name Elementy interfejsu
  @{
  */

Node * node_new(wchar_t character)
{
    Node *node = (Node *) malloc(sizeof(Node));

    node->value = character;
    node->parent = NULL;
    node->children = map_new();
    node->is_word = false;

    return node;
}

void node_done(Node *node)
{
    for (int i = 0; i < node_children_count(node); i++)
    {
        node_done(map_get_by_index(node->children, i));
    }

    map_done(node->children);
    free(node);
}

void node_add_child(Node *node, wchar_t character)
{
    Node *child = node_new(character);
    child->parent = node;
    if (!map_insert(node->children, character, child))
    {
        node_done(child);
    }
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

void node_get_hints(const Node *node, const wchar_t *word,
                    struct word_list *list)
{
    size_t len = wcslen(word);

    wchar_t remove[len], replace[len+1], add[len+2];
    wcscpy(remove, word+1);
    wcscpy(replace, word);
    wcscpy(add+1, word);

    get_hints_add(node, add, 0, list);

    for (size_t i = 0; i < len; i++)
    {
        if (i > 0) remove[i-1] = word[i-1];
        get_hints_remove(node, remove, i, list);

        get_hints_replace(node, replace, i, list);
        replace[i] = word[i];

        node = node_get_child(node, word[i]);
        if (node == NULL)
        {
            return;
        }

        add[i] = add[i+1];
        get_hints_add(node, add, i+1, list);
    }
}

int node_save(const Node *node, FILE* stream)
{
    for (int i = 0; i < node_children_count(node); i++)
    {
        Node *child = map_get_by_index(node->children, i);

        if (fprintf(stream, "%lc", child->value) < 0) return -1;
        if (node_is_word(child) && fprintf(stream, "%lc", L'*') < 0) return -1;
        if (node_save(child, stream) < 0) return -1;
        if (fprintf(stream, "%lc", L'^') < 0) return -1;
    }

    return 0;
}

/**@}*/
