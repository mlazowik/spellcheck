/** @file
    Interfejs reguł.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-26
 */

#ifndef __RULE_H__
#define __RULE_H__

#include "io.h"
#include "dictionary.h"
#include "vector.h"
#include "state.h"
#include <stdbool.h>
#include <wchar.h>

/**
  Struktura przechowująca regułę.
  */
typedef struct rule Rule;

/**
  Inicjalizacja reguły.
  Należy go zniszczyć za pomocą rule_done()
  @param[in] left Lewa strona reguły.
  @param[in] right Prawa strona reguły.
  @param[in] cost Koszt reguły.
  @param[in] flag Użyta flaga bądź jej brak.
  @return Nowa reguła.
  */
Rule * rule_new(const wchar_t *left, const wchar_t *right,
                int cost, enum rule_flag flag);

/**
  Destrukcja reguły.
  @param[in,out] rule Reguła.
  */
void rule_done(Rule *rule);

/**
  Zwraca koszt reguły.
  @param rule Reguła
  @return Koszt reguły.
  */
int rule_get_cost(Rule *rule);

/**
  Stwierdza, czy reguła pasuje do prefiksu słowa.
  @param rule Reguła.
  @param is_start Czy prefiks jest na początku słowa.
  @param word Słowo.
  @param word_len Długość słowa.
  @return Czy reguła pasuje do prefiksu słowa.
  */
bool rule_matches_prefix(Rule *rule, bool is_start, const wchar_t *word,
                         const size_t word_len);

/**
  Stosuje regułę do stanu.

  @param rule Reguła.
  @param state Stan.
  @param root Korzeń drzewa słownika.
  @return Wektor stanów.
  */
Vector * rule_apply(Rule *rule, State *state, Node *root);

/**
  Sprawdza, czy reguła sprłnia przyjęte założenia.
  @param[in] rule Reguła.
  @return Czy jest ok.
  */
bool rule_is_legal(Rule *rule);

/**
  Zapisuje regułę.
  @param[in] rule Reguła.
  @param io We/wy
  @return <0 jeśli operacja się nie powiedzie, 0 w p.p.
 */
int rule_save(const Rule *rule, IO *io);

/**
  Inicjuje i wczytuje regułę.
  Regułę tę należy zniszczyć za pomocą rule_done().
  @param[in,out] io We/wy.
  @return Nowa reguła lub NULL, jeśli operacja się nie powiedzie.
  */
Rule * rule_load(IO *io);

#endif /* __RULE_H__ */