/** @file
 Interfejs drzewa trie.

 @ingroup dictionary
 @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
 @copyright Uniwersytet Warszawski
 @date 2015-06-05
 */

#ifndef __TRIE_H__
#define __TRIE_H__

#include <wchar.h>
#include <stdbool.h>
#include <stdio.h>
#include "word_list.h"

/**
  Struktura przechowująca drzewo.
  */
typedef struct trie Trie;

/**
  Inicjalizacja drzewa.
  Należy je zniszczyć za pomocą trie_done()
  @return Nowe drzewo.
  */
Trie * trie_new();

/**
  Destrukcja drzewa.
  @param[in,out] trie Drzewo.
  */
void trie_done(Trie *trie);

/**
  Wstawia słowo do drzewa.
  @param[in,out] trie Drzewo.
  @param[in] word Wstawiane słowo.
  @return 0 jeśli słowo było już w drzewie, 1 jeśli udało się wstawić
  */
int trie_insert_word(Trie *trie, const wchar_t *word);

/**
  Usuwa słowo z drzewa.
  @param[in,out] trie Drzewo.
  @param[in] word Usuwane słowo.
  @return 1 jeśli się udało, 0 w p.p.
  */
int trie_delete_word(Trie *trie, const wchar_t *word);

/**
  Sprawdza, czy drzewo zawiera dane słowo.
  @param[in] trie Drzewo.
  @param[in] word Sprawdzane słowo.
  @return Wartość logiczna określająca czy słowo istnieje.
  */
bool trie_has_word(const Trie *trie, const wchar_t *word);

/**
  Dodaje podpowiedzi dla danego słowa do drzewa podpowiedzi.
  @param[in] trie Węzeł.
  @param[in] word Sprawdzane słowo.
  @param[in,out] hints Drzewo podpowiedzi.
  */
void trie_get_hints(const Trie *trie, const wchar_t *word, Trie *hints);

/**
  Zwraca listę słów zapisanych w drzewie.
  @param[in] trie Drzewp
  @param[in,out] list Lista słów w drzewie.
  */
void trie_to_word_list(const Trie *trie, struct word_list *list);

/**
  Zapisuje drzewo.
  @param[in] trie Drzewo.
  @param[in,out] stream Strumień, gdzie ma być zapisane drzewo.
  @return <0 jeśli operacja się nie powiedzie, 0 w p.p.
 */
int trie_save(const Trie *trie, FILE* stream);

/**
  Inicjuje i wczytuje drzewo.
  Drzewo to należy zniszczyć za pomocą trie_done().
  @param[in,out] stream Strumień, skąd ma być wczytany drzewo.
  @return Nowe drzewo lub NULL, jeśli operacja się nie powiedzie.
 */
Trie * trie_load(FILE* stream);

#endif /* __TRIE_H__ */
