/** @file
    Interfejs dynamicznego zbioru.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-26
 */

#ifndef __SET_H__
#define __SET_H__

#include <wchar.h>

/// Typ funkcji porównującej elemnty zbioru
typedef int (*set_cmp_func)(void *, void *);

/// Typ funkcji niszczącej element zbioru
typedef void (*set_free_el_func)(void *);

/**
  Struktura przechowująca zbiór.
  */
typedef struct set Set;

/**
  Inicjalizacja zbioru.
  Należy go zniszczyć za pomocą set_done()
  @return Nowy zbiór.
  */
Set * set_new(set_cmp_func cmp, set_free_el_func free_el);

/**
  Destrukcja zbioru.
  @param[in,out] set Zbiór.
  */
void set_done(Set *set);

/**
  Usuwa wszystkie elementy zbioru niszcząc je.
  @param set Zbiór.
  */
void set_clear(Set *set);

/**
  Dodaje element do zbioru.
  @param[in,out] set Zbiór.
  @param[in] el Dodawany element.
  @return 0 jeśli element o danym kluczu już istnieje, 1 w p.p.
  */
int set_insert(Set *set, void *el);

/**
  Dodaje element na koniec zbioru.
  @param[in,out] set Zbiór.
  @param[in] el Dodawany element.
  @return 0 jeśli element o danym kluczu już istnieje, 1 w p.p.
  */
int set_insert_at_end(Set *set, void *el);

/**
  Usuwa element ze zbioru.
  @param[in,out] set Zbiór.
  @param[in] el Usuwany element.
  @return 0 jeśli element nie istnieje, 1 w p.p.
  */
int set_delete(Set *set, void *el);

/**
  Zwraca element ze zbioru.
  @param[in] set Zbiór.
  @param[in] el Szukany element.
  @return Element zbioru lub NULL, jeśli nie istnieje.
  */
void * set_find(const Set *set, void *el);

/**
  Zwraca element o danym indeksie ze zbioru.
  @param[in] set Zbiór.
  @param[in] index Indeks.
  @return Element zbioru lub NULL, jeśli nie istnieje.
  */
void * set_get_by_index(const Set *set, const int index);

/**
  Zwraca liczę elementów w ziorze.
  @param[in] set Zbiór.
  @return Liczba elementów zbioru.
  */
const size_t set_size(const Set *set);

#endif /* __SET_H__ */
