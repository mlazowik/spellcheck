/** @file
    Implementacja listy słów.

    @ingroup dictionary
    @author Jakub Pawlewicz <pan@mimuw.edu.pl>
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-06-05
 */

#include "word_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

/**
  Początkowa pojemność.
  */
#define MINIMAL_CAPACITY 4

/**
  Początkowa pojemność bufora.
  */
#define MINIMAL_BUFFER_CAPACITY 20

/**
  Współczynnik przyrostu pamięci.
  Więcej o jego wyborze [na githubie Facebooka][fb growth factor].

  [fb growth factor]: https://github.com/facebook/folly/blob/master/folly/docs/FBVector.md#memory-handling)
  */
#define GROWTH_FACTOR 1.5

/** @name Funkcje pomocnicze
 @{
 */

/*
 Zmienia pojemność tablicy wskaźników na słowa.
 */
static void change_array_capacity(struct word_list *list,
                                  const size_t new_capacity)
{
    void *new_data = realloc(list->array, sizeof(wchar_t*) * new_capacity);
    if (!new_data)
    {
        fprintf(stderr, "Failed to reallocate memory for word list\n");
        exit(EXIT_FAILURE);
    }

    list->array = new_data;
    list->capacity = new_capacity;
}

/*
 Aktualizuje wskaźniki na słowa w buforze.
 */
static void update_word_pointers(struct word_list *list, ptrdiff_t delta)
{
    for (size_t i = 0; i < list->size; i++)
    {
        list->array[i] += delta;
    }
}

/*
 Zmienia pojemność bufora.
 */
static void change_buffer_capacity(struct word_list *list,
                                   const size_t new_capacity)
{
    void *new_data = realloc(list->buffer, sizeof(wchar_t) * new_capacity);
    if (!new_data)
    {
        fprintf(stderr, "Failed to reallocate memory for word list buffer\n");
        exit(EXIT_FAILURE);
    }

    update_word_pointers(list, (wchar_t*) new_data - list->buffer);

    list->buffer = new_data;
    list->buffer_capacity = new_capacity;
}

/*
 Zwiększa pojemność jeśli nie starczyłoby jej na dodanie kolejnego słowa.
 */
static void increase_array_capacity_if_needed(struct word_list *list)
{
    if (list->size == list->capacity)
    {
        float new_capacity = (float)(list->capacity) * GROWTH_FACTOR;
        size_t new_capacity_rounded = (size_t)(new_capacity + 1);

        change_array_capacity(list, new_capacity_rounded);
    }
}

/*
 Zwiększa pojemność bufora jeśli nie starczyłoby jej na dodanie kolejnego słowa.
 */
static void increase_buffer_capacity_if_needed(struct word_list *list,
                                               const size_t next_word_length)
{
    while (list->buffer_size + next_word_length >= list->buffer_capacity)
    {
        float new_capacity = (float)(list->buffer_capacity) * GROWTH_FACTOR;
        size_t new_capacity_rounded = (size_t)(new_capacity + 1);

        change_buffer_capacity(list, new_capacity_rounded);
    }
}

/*
 Zwiększa pojemność listy jeśli nie starczyłoby jej na dodanie kolejnego słowa.
 */
static void increase_capacity_if_needed(struct word_list *list,
                                        const size_t next_word_length)
{
    increase_array_capacity_if_needed(list);
    increase_buffer_capacity_if_needed(list, next_word_length);
}

/*
 Porwónuje słowa, uwzględniając lcale.
 */
static int word_compare(const void *a, const void *b)
{
    wchar_t *_a = *(wchar_t**)a;
    wchar_t *_b = *(wchar_t**)b;

    return wcscoll(_a, _b);
}

/**@}*/
/** @name Elementy interfejsu 
   @{
 */

void word_list_init(struct word_list *list)
{
    list->size = 0;
    list->capacity = MINIMAL_CAPACITY;
    list->array = malloc(sizeof(wchar_t*) * list->capacity);

    list->buffer_size = 0;
    list->buffer_capacity = MINIMAL_BUFFER_CAPACITY;
    list->buffer = malloc(sizeof(wchar_t) * list->buffer_capacity);

    if (!list->array || !list->buffer)
    {
        fprintf(stderr, "Failed to allocate memory for word_list\n");
        exit(EXIT_FAILURE);
    }
}

void word_list_done(struct word_list *list)
{
    free(list->array);
    free(list->buffer);
}

int word_list_add(struct word_list *list, const wchar_t *word)
{
    size_t len = wcslen(word) + 1;

    increase_capacity_if_needed(list, len);

    wchar_t *pos = list->buffer + list->buffer_size;
    list->array[list->size++] = pos;
    wcscpy(pos, word);
    list->buffer_size += len;

    return 1;
}

void word_list_sort(struct word_list *list)
{
    qsort(list->array, list->size, sizeof(wchar_t*), word_compare);
}

/**@}*/
