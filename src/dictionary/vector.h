/** @file
    Interfejs wektora.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-21
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdlib.h>

/// Typ funkcji niszczącej element wektora
typedef void (*vector_free_el_func)(void *);

/**
  Struktura przechowująca tablicę.
  */
typedef struct vector Vector;

/**
  Inicjalizacja wektora.
  Należy go zniszczyć za pomocą vector_done()
  @return Nowy wektor.
  */
Vector * vector_new(vector_free_el_func free_el);

/**
  Destrukcja wektora.
  @param[in,out] vector Wektor.
  */
void vector_done(Vector *vector);

/**
  Dodaje element przed wskazaną pozycją.
  @param[in,out] vector Wektor.
  @param[in] index Indeks.
  @param[in] el Dodawany element.
  */
void vector_insert(Vector *vector, const int index, void *el);

/**
  Usuwa element o danym indeksie z wektora.
  @param[in,out] vector Wektor.
  @param[in] index Indeks usuwanego elementu.
  */
void vector_delete(Vector *vector, const int index);

/**
  Dodaje element na koniec wektora.
  @param[in,out] vector Wektor.
  @param[in] el Dodawany element.
  */
void vector_push_back(Vector *vector, void *el);

/**
  Usuwa ostatni element z wektora.
  @param[in,out] vector Wektor.
  */
void vector_pop_back(Vector *vector);

/**
  Usuwa wszystkie el. wektora.
  @param[in,out] vector Wektor.
  */
void vector_clear(Vector *vector);

/**
  Zwraca element o danym indeksie z wektora.
  @param[in] vector Wektor.
  @param[in] index Indeks.
  @return Element tablicy lub NULL, jeśli nie istnieje.
  */
void * vector_get_by_index(const Vector *vector, const int index);

/**
  Zwraca liczę elementów w wektorze.
  @param[in] vector Wektor.
  @return Liczba elementów tablicy.
  */
size_t vector_size(const Vector *vector);

#endif /* __VECTOR_H__ */
