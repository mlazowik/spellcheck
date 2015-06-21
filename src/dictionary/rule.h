/** @file
    Interfejs reguł.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-21
 */

#ifndef __RULE_H__
#define __RULE_H__

#include "io.h"
#include <stdbool.h>
#include <wchar.h>

/**
  Struktura przechowująca regułę.
  */
typedef struct rule Rule;

/**
  Reprezentuje flagę przypisaną regule.
  */
enum rule_flag
{
    RULE_NORMAL, ///< Brak flagi.
    RULE_BEGIN,  ///< Flaga b.
    RULE_END,    ///< Flaga e.
    RULE_SPLIT   ///< Flaga s.
};

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

#endif /* __RULE_H__ */