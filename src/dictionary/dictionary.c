/** @file
    Implementacja słownika.
    Słownik przechowuje drzewo trie.

    @ingroup dictionary
    @author Jakub Pawlewicz <pan@mimuw.edu.pl>
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-06-19
 */

#include "dictionary.h"
#include "trie.h"
#include "io.h"
#include "conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <argz.h>

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
 Zwraca, czy plik jest obecnym lub nadrzędnym katalogiem.
 */
static bool file_is_current_or_parent_dir(char *filename)
{
    return (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0);
}

static FILE * open_dict_file(const char *lang, char *permissions)
{
    char path[strlen(CONF_PATH) + strlen(lang) + 1];

    strcpy(path, CONF_PATH);
    strcat(path, "/");
    strcat(path, lang);

    return fopen(path, permissions);
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
    IO *io = io_new(stdin, stream, stderr);

    int ret = trie_save(dict->trie, io);

    io_done(io);

    return ret;
}

struct dictionary * dictionary_load(FILE* stream)
{
    IO *io = io_new(stream, stdout, stderr);

    Trie *trie = trie_load(io);

    io_done(io);

    if (trie == NULL) return NULL;

    struct dictionary *dict = dictionary_new();

    trie_done(dict->trie);
    dict->trie = trie;

    return dict;
}

void dictionary_hints(const struct dictionary *dict, const wchar_t* word,
                      struct word_list *list)
{
    Trie *hints = trie_new();
    word_list_init(list);

    trie_get_hints(dict->trie, word, hints);
    trie_to_word_list(hints, list);

    trie_done(hints);

    word_list_sort(list);
}

int dictionary_lang_list(char **list, size_t *list_len)
{
    *list = NULL;
    *list_len = 0;

    DIR *dir = opendir(CONF_PATH);

    if (dir == NULL) return -1;

    struct dirent *dirent;

    while ((dirent = readdir(dir)) != NULL) {
        if (!file_is_current_or_parent_dir(dirent->d_name)
            && argz_add(list, list_len, dirent->d_name) != 0)
        {
            free(*list);
            *list = NULL;
            *list_len = 0;
            return -1;
        }
    }

    closedir(dir);

    return 0;
}

struct dictionary * dictionary_load_lang(const char *lang)
{
    FILE *dict_file;

    if (!(dict_file = open_dict_file(lang, "r+"))) return NULL;

    struct dictionary *dict = dictionary_load(dict_file);

    fclose(dict_file);

    return dict;
}

int dictionary_save_lang(const struct dictionary *dict, const char *lang)
{
    mkdir(CONF_PATH, 0700);

    FILE *dict_file;

    if (!(dict_file = open_dict_file(lang, "w+"))) return -1;

    int ret = dictionary_save(dict, dict_file);

    fclose(dict_file);

    return ret;
}

/**@}*/
