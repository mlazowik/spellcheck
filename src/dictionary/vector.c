/** @file
    Implementacja wektora.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-21
 */

#include "vector.h"
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
  Struktura przechowująca wektor.
  */
struct vector
{
    /// Rozmiar (liczba elementów)
    size_t size;
    /// Pojemność (aktualnie zarezerwowana pamięć)
    size_t capacity;

    /// Funkcja niszcząca element wektora.
    vector_free_el_func free_el;

    /// Dane.
    void **data;
};

/** @name Funkcje pomocnicze
  @{
  */

/*
 Zmienia pojemność wektora.
 */
static void change_capacity(Vector *vector, const size_t new_capacity)
{
    void **new_data = realloc(vector->data, sizeof(void*) * new_capacity);
    if (!new_data)
    {
        fprintf(stderr, "Failed to reallocate memory for vector\n");
        exit(EXIT_FAILURE);
    }

    vector->data = new_data;
    vector->capacity = new_capacity;
}

/*
 Zwiększa pojemność jeśli nie starczyłoby jej na dodanie kolejnego elementu.
 */
static void increase_capacity_if_needed(Vector *vector)
{
    if (vector->size == vector->capacity)
    {
        float new_capacity = (float)(vector->capacity) * GROWTH_FACTOR;
        size_t new_capacity_rounded = (size_t)(new_capacity + 1);

        change_capacity(vector, new_capacity_rounded);
    }
}

/*
 Zmniejsza pojemność jeśli jest wyraźnie za duża.
 Wyraźne za duża tzn. elementów wektora jest mniej niż 1 / 2*(współczynnik
 przyrostu pamięci).
 */
static void decrease_capacity_if_needed(Vector *vector)
{
    if (vector->size < vector->capacity / (2 * GROWTH_FACTOR))
    {
        size_t new_capacity = vector->capacity / GROWTH_FACTOR;
        if (new_capacity < MINIMAL_CAPACITY)
        {
            new_capacity = MINIMAL_CAPACITY;
        }

        change_capacity(vector, new_capacity);
    }
}

/*
 Przesuwa elementy tablicy od miejsca `start` o `offest` elementów.
 */
static void shift_array(Vector *vector, const int start, const int offset)
{
    memmove(
        &(vector->data[start + offset]),
        &(vector->data[start]),
        (vector->size - start) * sizeof(void*)
    );
}

/**@}*/
/** @name Elementy interfejsu
  @{
  */

Vector * vector_new(vector_free_el_func free_el)
{
    Vector *vector = (Vector *) malloc(sizeof(Vector));

    vector->size = 0;
    vector->capacity = MINIMAL_CAPACITY;

    vector->free_el = free_el;

    vector->data = malloc(sizeof(void*) * vector->capacity);
    if (!vector->data)
    {
        fprintf(stderr, "Failed to allocate memory for vector\n");
        exit(EXIT_FAILURE);
    }

    return vector;
}

void vector_done(Vector *vector) {
    free(vector->data);
    free(vector);
}

void vector_insert(Vector *vector, const int index, void *el) {
    increase_capacity_if_needed(vector);

    assert(index >= 0 && index <= vector->size);

    if (index < vector->size)
    {
        shift_array(vector, index, 1);
    }

    vector->data[index] = el;
    vector->size++;
}

void vector_delete(Vector *vector, const int index) {
    assert(index >= 0 && index < vector->size);

    vector->free_el(vector->data[index]);

    if (index + 1 < vector->size)
    {
        shift_array(vector, index+1, -1);
    }

    vector->size--;

    decrease_capacity_if_needed(vector);
}

void vector_push_back(Vector *vector, void *el)
{
    vector_insert(vector, vector->size, el);
}

void vector_pop_back(Vector *vector)
{
    vector_delete(vector, vector->size - 1);
}

void vector_clear(Vector *vector)
{
    while (vector->size > 0) vector_pop_back(vector);
}

void * vector_get_by_index(const Vector *vector, const int index)
{
    if (index < 0 || index >= vector->size)
    {
        return NULL;
    }

    return vector->data[index];
}

size_t vector_size(const Vector *vector)
{
    return vector->size;
}

/**@}*/
