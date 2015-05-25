/** @file
    Implementacja dynamicznego zbioru węzłów indeksowanego znakami.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-05-24
 */

#include "map.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
  Minimalna i początkowa pojemność.
  */
#define MAP_MINIMAL_CAPACITY 20

/**
  Współczynnik przyrostu pamięci.
  Więcej o jego wyborze:
  https://github.com/facebook/folly/blob/master/folly/docs/FBVector.md#memory-handling
  */
#define GROWTH_FACTOR 1.5

/**
  Struktura przechowująca parę <klucz, wartość>
  */
typedef struct
{
    /// Klucz.
    wchar_t key;
    /// Wartość.
    Node *val;
} MapEntry;

/**
  Struktura przechowująca zbiór.
  */
struct map
{
    /// Rozmiar (liczba elementów)
    size_t size;
    /// Pojemność (aktualnie zarezerwowana pamięć)
    size_t capacity;
    /// Dane.
    MapEntry *data;
};

/** @name Funkcje pomocnicze
  @{
  */
/**
  Zmienia pojemność zbioru.
  @param[in,out] map Zbiór.
  @param[in] new_capacity Docelowa pojemność.
  */
static void change_capacity(Map *map, size_t new_capacity)
{
    void *new_data = realloc(map->data, sizeof(MapEntry) * new_capacity);
    if (!new_data)
    {
        fprintf(stderr, "Failed to reallocate memory for Map");
        exit(EXIT_FAILURE);
    }

    map->data = new_data;
    map->capacity = new_capacity;
}

/**
  Zwiększa pojemność jeśli nie starczyłoby jej na dodanie kolejnego elementu.
  @param[in,out] map Zbiór.
  */
static void increase_capacity_if_needed(Map *map)
{
    if (map->size == map->capacity)
    {
        float new_capacity = (float)(map->capacity) * GROWTH_FACTOR;
        size_t new_capacity_rounded = (size_t)new_capacity + 1;

        change_capacity(map, new_capacity_rounded);
    }
}

/**
  Zmniejsza pojemność jeśli jest wyraźnie za duża.
  Wyraźne za duża tzn. elementów zbioru jest mniej niż 1 / 2*(współczynnik
  przyrostu pamięci).
  @param[in,out] map Zbiór.
  */
static void decrease_capacity_if_needed(Map *map)
{
    if (map->size < map->capacity / (2 * GROWTH_FACTOR))
    {
        size_t new_capacity = map->capacity / GROWTH_FACTOR;
        if (new_capacity < MAP_MINIMAL_CAPACITY)
        {
            new_capacity = MAP_MINIMAL_CAPACITY;
        }

        change_capacity(map, new_capacity);
    }
}

/**
  Przesuwa elementy tablicy.
  @param[in,out] map Zbiór.
  @param[in] start Indeks pierwszego przesuwanego elementu.
  @param[in] offset O ile miejsc przesunąć.
  */
static void shift_array(Map *map, int start, int offset)
{
    memmove(
        &(map->data[start + offset]),
        &(map->data[start]),
        (map->size - start) * sizeof(MapEntry)
    );
}

/**
  Porównuje klucz pod daną pozycją w zbiorze z podanym kluczem.
  @param[in,out] map Zbiór.
  @param[in] pos Pozycja w zbiorze, pod którą jest pierwszy porównywany klucz.
  @param[in] key Drugi porównywany klucz.
  @return >0 jeśli pierwszy jest większy, 0 jeśli równe,
    <0 jeśli drugi jest większy.
  */
static int compare(const Map *map, int pos, wchar_t key)
{
    assert(pos < map->size);

    wchar_t lhs[2], rhs[2];

    lhs[0] = (map->data[pos]).key;
    lhs[1] = L'\0';

    rhs[0] = key;
    rhs[1] = L'\0';

    return wcscoll(lhs, rhs);
}

/**
  Wyszukuje pozycję danego klucza, lub pozycję na którą należy go wstawić.
  Złożoność: O(nlogn)
  @param[in,out] map Zbiór.
  @param[in] key Klucz.
  @return Pozycja klucza, lub miejsce w którym powinien się znaleźć.
  */
static int find_position(const Map *map, wchar_t key)
{
    int l = 0, r = map->size, mid = (l + r) / 2;

    while (r - l > 0)
    {
        if (compare(map, mid, key) > 0)
        {
            l = mid + 1;
        }
        else
        {
            r = mid;
        }
        mid = (l + r) / 2;
    }

    return l;
}

/**@}*/
/** @name Elementy interfejsu
  @{
  */

Map * map_new(void)
{
    Map *map = (Map *) malloc(sizeof(Map));

    map->size = 0;
    map->capacity = MAP_MINIMAL_CAPACITY;
    map->data = malloc(sizeof(MapEntry) * map->capacity);

    return map;
}

void map_done(Map *map) {
    free(map->data);
    free(map);
}

int map_insert(Map *map, wchar_t key, Node *val) {
    increase_capacity_if_needed(map);

    int pos = find_position(map, key);
    if (pos < map->size && compare(map, pos, key) == 0)
    {
        return 0;
    }

    if (pos < map->size)
    {
        shift_array(map, pos, 1);
    }

    (map->data[pos]).key = key;
    (map->data[pos]).val = val;
    map->size++;

    return 1;
}

int map_delete(Map *map, wchar_t key) {
    int pos = find_position(map, key);

    if (pos == map->size || compare(map, pos, key) != 0)
    {
        return 0;
    }

    node_done((map->data[pos].val));

    if (pos + 1 < map->size)
    {
        shift_array(map, pos+1, -1);
    }

    map->size--;

    decrease_capacity_if_needed(map);

    return 1;
}

Node * map_find(const Map *map, wchar_t key) {
    int pos = find_position(map, key);

    if (pos == map->size || compare(map, pos, key) != 0)
    {
        return NULL;
    }

    return (map->data[pos]).val;
}

Node * map_get_by_index(const Map *map, int index) {
    if (index < 0 || index > map->size) {
        return NULL;
    }

    return (map->data[index]).val;
}

size_t map_size(const Map *map)
{
    return map->size;
}

/**@}*/
