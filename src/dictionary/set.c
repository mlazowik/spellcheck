/** @file
    Implementacja dynamicznego zbioru.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-19
 */

#include "set.h"
#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
  Struktura przechowująca zbiór.
  */
struct set
{
    /// Funkcja porównująca elementy
    set_cmp_func cmp;

    /// Wektor danych.
    Vector *data;
};

/** @name Funkcje pomocnicze
  @{
  */

/*
 Wyszukuje pozycję danego elemntu, lub pozycję na którą należy go wstawić.
 Złożoność: O(nlogn)
 */
static int find_position(const Set *set, void *el)
{
    int l = 0, r = set_size(set), mid = (l + r) / 2;

    while (r - l > 0)
    {
        if (set->cmp(set_get_by_index(set, mid), el) >= 0)
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

    set->cmp = cmp;

    set->data = vector_new(free_el);

    return set;
}

void set_done(Set *set) {
    vector_done(set->data);
    free(set);
}

void set_clear(Set *set)
{
    vector_clear(set->data);
}

int set_insert(Set *set, void *el) {
    int pos = find_position(set, el);
    if (pos < set_size(set) && set->cmp(set_get_by_index(set, pos), el) == 0)
    {
        return 0;
    }

    vector_insert(set->data, pos, el);

    return 1;
}

int set_delete(Set *set, void *el) {
    int pos = find_position(set, el);

    if (pos == set_size(set) || set->cmp(set_get_by_index(set, pos), el) != 0)
    {
        return 0;
    }

    vector_delete(set->data, pos);

    return 1;
}

void * set_find(const Set *set, void *el)
{
    int pos = find_position(set, el);

    if (pos == set_size(set) || set->cmp(set_get_by_index(set, pos), el) != 0)
    {
        return NULL;
    }

    return set_get_by_index(set, pos);
}

void * set_get_by_index(const Set *set, const int index)
{
    if (index < 0 || index >= set_size(set))
    {
        return NULL;
    }

    return vector_get_by_index(set->data, index);
}

size_t set_size(const Set *set)
{
    return vector_size(set->data);
}

/**@}*/
