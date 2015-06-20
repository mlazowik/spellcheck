/** @file
    Implementacja dynamicznego zbioru.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-19
 */

#include "set.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
  Minimalna i początkowa pojemność.
  */
#define MINIMAL_CAPACITY 2

/**
  Współczynnik przyrostu pamięci.
  Więcej o jego wyborze [na githubie Facebooka][fb growth factor].

  [fb growth factor]: https://github.com/facebook/folly/blob/master/folly/docs/FBVector.md#memory-handling)
  */
#define GROWTH_FACTOR 1.5

/**
  Struktura przechowująca zbiór.
  */
struct set
{
    /// Rozmiar (liczba elementów)
    size_t size;
    /// Pojemność (aktualnie zarezerwowana pamięć)
    size_t capacity;

    /// Funkcja porównująca elementy
    set_cmp_func cmp;
    /// Funkcja niszcząca element zbioru
    set_free_el_func free_el;

    /// Dane.
    void **data;
};

/** @name Funkcje pomocnicze
  @{
  */

/*
 Zmienia pojemność zbioru.
 */
static void change_capacity(Set *set, const size_t new_capacity)
{
    void **new_data = realloc(set->data, sizeof(void*) * new_capacity);
    if (!new_data)
    {
        fprintf(stderr, "Failed to reallocate memory for set\n");
        exit(EXIT_FAILURE);
    }

    set->data = new_data;
    set->capacity = new_capacity;
}

/*
 Zwiększa pojemność jeśli nie starczyłoby jej na dodanie kolejnego elementu.
 */
static void increase_capacity_if_needed(Set *set)
{
    if (set->size == set->capacity)
    {
        float new_capacity = (float)(set->capacity) * GROWTH_FACTOR;
        size_t new_capacity_rounded = (size_t)(new_capacity + 1);

        change_capacity(set, new_capacity_rounded);
    }
}

/*
 Zmniejsza pojemność jeśli jest wyraźnie za duża.
 Wyraźne za duża tzn. elementów zbioru jest mniej niż 1 / 2*(współczynnik
 przyrostu pamięci).
 */
static void decrease_capacity_if_needed(Set *set)
{
    if (set->size < set->capacity / (2 * GROWTH_FACTOR))
    {
        size_t new_capacity = set->capacity / GROWTH_FACTOR;
        if (new_capacity < MINIMAL_CAPACITY)
        {
            new_capacity = MINIMAL_CAPACITY;
        }

        change_capacity(set, new_capacity);
    }
}

/*
 Przesuwa elementy tablicy od miejsca `start` o `offest` elementów.
 */
static void shift_array(Set *set, const int start, const int offset)
{
    memmove(
        &(set->data[start + offset]),
        &(set->data[start]),
        (set->size - start) * sizeof(void*)
    );
}

/*
 Wyszukuje pozycję danego elemntu, lub pozycję na którą należy go wstawić.
 Złożoność: O(nlogn)
 */
static int find_position(const Set *set, void *el)
{
    int l = 0, r = set->size, mid = (l + r) / 2;

    while (r - l > 0)
    {
        if (set->cmp(set->data[mid], el) >= 0)
        {
            r = mid;
        }
        else
        {
            l = mid + 1;
        }
        mid = (l + r) / 2;
    }

    return l;
}

/**@}*/
/** @name Elementy interfejsu
  @{
  */

Set * set_new(set_cmp_func cmp, set_free_el_func free_el)
{
    Set *set = (Set *) malloc(sizeof(Set));

    set->size = 0;
    set->capacity = MINIMAL_CAPACITY;

    set->cmp = cmp;
    set->free_el = free_el;

    set->data = malloc(sizeof(void*) * set->capacity);
    if (!set->data)
    {
        fprintf(stderr, "Failed to allocate memory for set\n");
        exit(EXIT_FAILURE);
    }

    return set;
}

void set_done(Set *set) {
    free(set->data);
    free(set);
}

int set_insert(Set *set, void *el) {
    increase_capacity_if_needed(set);

    int pos = find_position(set, el);
    if (pos < set->size && set->cmp(set->data[pos], el) == 0)
    {
        return 0;
    }

    if (pos < set->size)
    {
        shift_array(set, pos, 1);
    }

    set->data[pos] = el;
    set->size++;

    return 1;
}

int set_delete(Set *set, void *el) {
    int pos = find_position(set, el);

    if (pos == set->size || set->cmp(set->data[pos], el) != 0)
    {
        return 0;
    }

    set->free_el(set->data[pos]);

    if (pos + 1 < set->size)
    {
        shift_array(set, pos+1, -1);
    }

    set->size--;

    decrease_capacity_if_needed(set);

    return 1;
}

void * set_find(const Set *set, void *el)
{
    int pos = find_position(set, el);

    if (pos == set->size || set->cmp(set->data[pos], el) != 0)
    {
        return NULL;
    }

    return set->data[pos];
}

void * set_get_by_index(const Set *set, const int index)
{
    if (index < 0 || index >= set->size)
    {
        return NULL;
    }

    return set->data[index];
}

size_t set_size(const Set *set)
{
    return set->size;
}

/**@}*/
