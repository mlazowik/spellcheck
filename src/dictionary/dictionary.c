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

/*
 Przesuwa wskaźniki słów do pierwszego miejsca w którym znaki się różnią.
 */
static void skip_equal(const wchar_t **a, const wchar_t **b)
{
    while (**a == **b && **a != L'\0')
    {
        (*a)++;
        (*b)++;
    }
}

/*
 Zwraca czy słowo `a` można zamienić w `b` przez usunięcie znaku.
 */
static bool can_transform_by_delete(const wchar_t *a, const wchar_t *b)
{
    skip_equal(&a, &b);
    a++;
    skip_equal(&a, &b);
    return *a == L'\0' && *b == L'\0';
}

/*
 Zwraca czy słowo `a` można zamienić w `b` przez zamianę znaku.
 */
static bool can_transform_by_replace(const wchar_t *a, const wchar_t *b)
{
    skip_equal(&a, &b);
    a++; b++;
    skip_equal(&a, &b);
    return *a == L'\0' && *b == L'\0';
}

/**@}*/
/** @name Elementy interfejsu 
  @{
 */
struct dictionary * dictionary_new()
{
    struct dictionary *dict =
        (struct dictionary *) malloc(sizeof(struct dictionary));
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
    /*word_list_init(list);
    size_t wlen = wcslen(word);
    const wchar_t * const * a = word_list_get(&dict->list);
    for (size_t i = 0; i < word_list_size(&dict->list); i++)
    {
        size_t len = wcslen(a[i]);
        if (len == wlen - 1)
        {
            if (can_transform_by_delete(word, a[i]))
                word_list_add(list, a[i]);
        }
        else if (len == wlen)
        {
            if (can_transform_by_replace(word, a[i]))
                word_list_add(list, a[i]);
        }
        else if (len == wlen + 1)
        {
            if (can_transform_by_delete(a[i], word))
                word_list_add(list, a[i]);
        }
    }*/
}

/**@}*/
