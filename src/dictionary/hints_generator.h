/** @file
    Interfejs generatora podpowiedzi.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-22
 */

#ifndef __HINTS_GENERATOR_H__
#define __HINTS_GENERATOR_H__

#include "rule.h"
#include "node.h"

/**
  Struktura przechowująca regułę.
  */
typedef struct hints_generator Hints_Generator;

/**
  Inicjalizacja generatora podpowiedzi.
  Należy go zniszczyć za pomocą rule_done()
  @return Nowy generator podpowiedzi.
  */
Hints_Generator * hints_generator_new();

/**
  Destrukcja generatora podpowiedzi.
  @param[in,out] hints_generator Generator podpowiedzi.
  */
void hints_generator_done(Hints_Generator *generator);

/**
  Ustawia maksymalny koszt z jakim jest generowana podpowiedź.
  @param[in,out] hints_generator Generator podpowiedzi..
  @param[in] new_cost Nowy maksymalny koszt.
  @return Zwraca dotychczasowy maksymalny koszt.
  */
int hints_generator_max_cost(Hints_Generator *generator, int new_cost);

/**
  Usuwa wszystkie reguły.
  @param[in,out] hints_generator Generator podpowiedzi.
  */
void hints_generator_rule_clear(Hints_Generator *generator);

/**
  Dodaje nową regułę do generatora.
  @param[in,out] hints_generator Generator podpowiedzi.
  */
void hints_generator_rule_add(Hints_Generator *generator, Rule *rule);

/**
  Zapisuje generator podpowiedzi.
  @param[in] hints_generator Generator podpowiedzi.
  @param io We/wy
  @return <0 jeśli operacja się nie powiedzie, 0 w p.p.
 */
int hints_generator_save(const Hints_Generator *generator, IO *io);

/**
  Inicjuje i wczytuje generator podpowiedzi.
  Regułę tę należy zniszczyć za pomocą hints_generator_done().
  @param[in,out] io We/wy.
  @return Nowy generator podpowiedzi lub NULL, jeśli operacja się nie powiedzie.
  */
Hints_Generator * hints_generator_load(IO *io);

#endif /* HINTS_GENERATOR */