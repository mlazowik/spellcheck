/** @file
    Interfejs generatora podpowiedzi.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-26
 */

#ifndef __HINTS_GENERATOR_H__
#define __HINTS_GENERATOR_H__

#include "rule.h"
#include "node.h"
#include "word_list.h"

/**
  Struktura przechowująca regułę.
  */
typedef struct hints_generator Hints_Generator;

/**
  Inicjalizacja generatora podpowiedzi.
  Należy go zniszczyć za pomocą hints_generator_done()
  @return Nowy generator podpowiedzi.
  */
Hints_Generator * hints_generator_new();

/**
  Destrukcja generatora podpowiedzi.
  @param[in,out] gen Generator podpowiedzi.
  */
void hints_generator_done(Hints_Generator *gen);

/**
  Ustawia wierzchołek od którego zaczynają się podpowiedzi.
  @param[in,out] gen Generator podpowiedzi.
  @param[in] root Korzeń słownika.
  */
void hints_generator_set_root(Hints_Generator *gen, Node *root);

/**
  Ustawia maksymalny koszt z jakim jest generowana podpowiedź.
  @param[in,out] gen Generator podpowiedzi..
  @param[in] new_cost Nowy maksymalny koszt.
  @return Zwraca dotychczasowy maksymalny koszt.
  */
int hints_generator_max_cost(Hints_Generator *gen, int new_cost);

/**
  Tworzy możliwe podpowiedzi dla zadanego słowa.
  Jeżeli pojedyncza podpowiedź składa się z kilku słów,
  wtedy powinien być to jeden łańcuch znaków,
  w którym słowa są pooddzielane pojedynczymi spacjami.
  @param[in] gen Generator podpowiedzi.
  @param[in] word Szukane słowo.
  @param[in,out] list Lista, w której zostaną umieszczone podpowiedzi.
  */
void hints_generator_hints(Hints_Generator *gen, const wchar_t* word,
                           struct word_list *list);

/**
  Usuwa wszystkie reguły.
  @param[in,out] gen Generator podpowiedzi.
  */
void hints_generator_rule_clear(Hints_Generator *gen);

/**
  Dodaje nową regułę do generatora.
  @param[in,out] gen Generator podpowiedzi.
  @param[in] rule Reguła.
  */
void hints_generator_rule_add(Hints_Generator *gen, Rule *rule);

/**
  Zapisuje generator podpowiedzi.
  @param[in] gen Generator podpowiedzi.
  @param[in,out] io We/wy
  @return <0 jeśli operacja się nie powiedzie, 0 w p.p.
 */
int hints_generator_save(const Hints_Generator *gen, IO *io);

/**
  Inicjuje i wczytuje generator podpowiedzi.
  Regułę tę należy zniszczyć za pomocą hints_generator_done().
  @param[in,out] io We/wy.
  @return Nowy generator podpowiedzi lub NULL, jeśli operacja się nie powiedzie.
  */
Hints_Generator * hints_generator_load(IO *io);

#endif /* HINTS_GENERATOR */