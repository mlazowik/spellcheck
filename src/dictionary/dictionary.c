/** @file
  Implementacja słownika.
  Słownik przechowuje drzewo trie.

  @ingroup dictionary
  @author Jakub Pawlewicz <pan@mimuw.edu.pl>
  @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
  @copyright Uniwerstet Warszawski
  @date 2015-05-24
  @todo Napisać efektywną implementację.
 */

#include "dictionary.h"
#include "trie.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>

#define _GNU_SOURCE

/**
  Struktura przechowująca słownik.
 */
struct dictionary
{
    /// Drzewo
    Trie *trie;
};

/** @name Funkcje pomocnicze
  @{
 */

/*
 Czyszczenie pamięci słownika
 */
static void dictionary_free(struct dictionary *dict)
{
    trie_done(dict->trie);
}

/**@}*/
/** @name Elementy interfejsu 
  @{
 */
struct dictionary * dictionary_new()
{
    struct dictionary *dict =
        (struct dictionary *) malloc(sizeof(struct dictionary));
    if (!dict)
    {
        fprintf(stderr, "Failed to allocate memory for dictionary\n");
        exit(EXIT_FAILURE);
    }

    dict->trie = trie_new();

    return dict;
}

void dictionary_done(struct dictionary *dict)
{
    dictionary_free(dict);
    free(dict);
}

int dictionary_insert(struct dictionary *dict, const wchar_t *word)
{
    return trie_insert_word(dict->trie, word);
}

int dictionary_delete(struct dictionary *dict, const wchar_t *word)
{
    return trie_delete_word(dict->trie, word);
}

bool dictionary_find(const struct dictionary *dict, const wchar_t* word)
{
    return trie_has_word(dict->trie, word);
}

int dictionary_save(const struct dictionary *dict, FILE* stream)
{
    return trie_save(dict->trie, stream);
}

struct dictionary * dictionary_load(FILE* stream)
{
    struct dictionary *dict = dictionary_new();

    dict->trie = trie_load(stream);

    if (dict->trie == NULL)
    {
        dictionary_done(dict);
        return NULL;
    }

    return dict;
}

void dictionary_hints(const struct dictionary *dict, const wchar_t* word,
                      struct word_list *list)
{
    Trie *hints = trie_new();
    word_list_init(list);

    trie_get_hints(dict->trie, word, hints);
    trie_to_word_list(hints, list);
}

/**@}*/
