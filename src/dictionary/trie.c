/** @file
 Implementacja drzewa trie.

 @ingroup dictionary
 @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
 @copyright Uniwersytet Warszawski
 @date 2015-05-24
 */

#include "trie.h"
#include "node.h"
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

/**
 Struktura przechowująca drzewo.
 */
struct trie
{
    /// Korzeń.
    Node *root;
    /// Długość najdłuższego słowa jakie kiedykolwiek było w drzewie.
    size_t longest;
};

/** @name Funkcje pomocnicze
  @{
  */

/*
 Usuwa zbędne węzły idąc "w górę" drzewa od podanego węzła.
 */
static void remove_non_words(Node *node)
{
    while (node_children_count(node) == 0
           && !node_is_word(node)
           && node_get_parent(node) != NULL)
    {
        Node *parent = node_get_parent(node);
        node_remove_child(node_get_parent(node), node_get_key(node));
        node = parent;
    }
}

/**@}*/
/** @name Elementy interfejsu
 @{
 */

Trie * trie_new(void)
{
    Trie *trie = (Trie *) malloc(sizeof(Trie));
    if (!trie)
    {
        fprintf(stderr, "Failed to allocate memory for trie\n");
        exit(EXIT_FAILURE);
    }

    trie->root = node_new(L'\0');
    trie->longest = 0;

    return trie;
}

void trie_done(Trie *trie)
{
    node_done(trie->root);
    free(trie);
}

int trie_insert_word(Trie *trie, const wchar_t *word)
{
    Node *current_node = trie->root;
    size_t word_length = wcslen(word);

    for (int i = 0; i < word_length; i++)
    {
        node_add_child(current_node, word[i]);
        current_node = node_get_child(current_node, word[i]);
    }

    if (node_is_word(current_node))
    {
        return 0;
    }

    node_set_is_word(current_node, true);

    if (word_length > trie->longest) trie->longest = word_length;

    return 1;
}

int trie_delete_word(Trie *trie, const wchar_t *word)
{
    Node *current_node = trie->root;
    size_t word_length = wcslen(word);

    for (int i = 0; i < word_length; i++)
    {
        current_node = node_get_child(current_node, word[i]);
        if (current_node == NULL)
        {
            return 0;
        }
    }

    if (!node_is_word(current_node))
    {
        return 0;
    }

    node_set_is_word(current_node, false);
    remove_non_words(current_node);

    return 1;
}

bool trie_has_word(const Trie *trie, const wchar_t *word)
{
    return node_has_word(trie->root, word);
}

void trie_get_hints(const Trie *trie, const wchar_t *word, Trie *hints)
{
    node_get_hints(trie->root, word, hints);
}

void trie_to_word_list(const Trie *trie, struct word_list *list)
{
    wchar_t prefix[trie->longest + 1];
    node_add_words_to_list(trie->root, prefix, 0, list);
}

int trie_save(const Trie *trie, FILE* stream)
{
    return node_save(trie->root, stream);
}

Trie * trie_load(FILE* stream)
{
    Trie *trie = trie_new();
    Node *node = trie->root;

    wchar_t c;
    while ((c = fgetwc(stream)) != WEOF)
    {
        if (c == L'*')
        {
            node_set_is_word(node, true);
        }
        else if (c == L'^')
        {
            node = node_get_parent(node);
            if (node == NULL)
            {
                trie_done(trie);
                return NULL;
            }
        }
        else
        {
            if (!iswalpha(c))
            {
                trie_done(trie);
                return NULL;
            }
            node_add_child(node, c);
            node = node_get_child(node, c);
        }
    }

    if (ferror(stream))
    {
        trie_done(trie);
        return NULL;
    }

    return trie;
}

/**@}*/
