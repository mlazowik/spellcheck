/** @file
    Interfejs dynamicznego zbioru węzłów indeksowanego znakami.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-05-24
 */

#ifndef __MAP_H__
#define __MAP_H__

#include "node.h"
#include <wchar.h>

/**
  Struktura przechowująca zbiór.
  */
typedef struct map Map;

/**
  Inicjalizacja zbioru.
  Należy go zniszczyć za pomocą map_done()
  @return Nowy zbiór.
  */
Map * map_new(void);

/**
  Destrukcja zbioru.
  @param[in,out] map Zbiór.
  */
void map_done(Map *map);

/**
  Dodaje element do zbioru.
  @param[in,out] map Zbiór.
  @param[in] key Klucz węzła.
  @param[in] val Dodawany węzeł.
  @return 0 jeśli element o danym kluczu już istnieje, 1 w p.p.
  */
int map_insert(Map *map, wchar_t key, Node *val);

/**
  Usuwa element o danym kluczu ze zbioru.
  @param[in,out] map Zbiór.
  @param[in] key Klucz usuwanego węzeła.
  @return 0 jeśli element nie istnieje, 1 w p.p.
  */
int map_delete(Map *map, wchar_t key);

/**
  Zwraca element o danym kluczu ze zbioru.
  @param[in,out] map Zbiór.
  @param[in] key Klucz szukanego węzeła.
  @return Element zbioru lub NULL, jeśli nie istnieje.
  */
Node * map_find(const Map *map, wchar_t key);

/**
  Zwraca element o danym indeksie ze zbioru.
  @param[in,out] map Zbiór.
  @param[in] index Indeks.
  @return Element zbioru lub NULL, jeśli nie istnieje.
  */
Node * map_get_by_index(const Map *map, int index);

/**
  Zwraca liczę elementów w ziorze.
  @param[in] map Zbiór.
  @return Liczba elementów zbioru.
  */
size_t map_size(const Map *map);

#endif /* __MAP_H__ */
