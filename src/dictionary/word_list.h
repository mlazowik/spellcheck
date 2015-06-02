/** @file
    Interfejs listy słów.

    @ingroup dictionary
    @author Jakub Pawlewicz <pan@mimuw.edu.pl>
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-06-02
 */

#ifndef __WORD_LIST_H__
#define __WORD_LIST_H__

#include <wchar.h>

/**
  Struktura przechowująca listę słów.
  Należy używać funkcji operujących na strukturze,
  gdyż jej implementacja może się zmienić.
  */
struct word_list
{
    /// Liczba słów.
    size_t size;
    /// Pojemność listy słów
    size_t capacity;
    /// Łączna liczba znaków.
    size_t buffer_size;
    /// Pojemność bufora
    size_t buffer_capacity;
    /// Tablica słów.
    const wchar_t **array;
    /// Bufor, w którym pamiętane są słowa.
    wchar_t *buffer;
};

/**
  Inicjuje listę słów.
  @param[in,out] list Lista słów.
  */
void word_list_init(struct word_list *list);

/**
  Destrukcja listy słów.
  @param[in,out] list Lista słów.
  */
void word_list_done(struct word_list *list);

/**
  Dodaje słowo do listy.
  @param[in,out] list Lista słów.
  @param[in] word Dodawane słowo.
  @return 1 jeśli się udało, 0 w p.p.
  */
int word_list_add(struct word_list *list, const wchar_t *word);

/**
  Zwraca liczę słów w liście.
  @param[in] list Lista słów.
  @return Liczba słów w liście.
  */
static inline
size_t word_list_size(const struct word_list *list)
{
    return list->size;
}

/**
  Zwraca tablicę słów w liście.
  @param[in] list Lista słów.
  @return Tablica słów.
  */
static inline
const wchar_t * const * word_list_get(const struct word_list *list)
{
    return list->array;
}

#endif /* __WORD_LIST_H__ */
