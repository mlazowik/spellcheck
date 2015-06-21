/** @file
    Implementacja dynamicznego zbioru.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-21
 */

#include "rule.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
  Struktura przechowująca regułę.
  */
struct rule
{
    wchar_t *left;          ///< Lewa strona reguły.
    size_t left_len;        ///< Długość lewej strony.
    wchar_t *right;         ///< Prawa strona reguły.
    size_t right_len;       ///< Długość prawej strony.
    int cost;               ///< Koszt reguły.
    enum rule_flag flag;    ///< Użyta flaga bądź jej brak.
};

/** @name Funkcje pomocnicze
  @{
  */

/*
 malloc opakowany w obsługę błedu
 */
void * emalloc(size_t el_size)
{
    void *ret = malloc(el_size);
    if (!ret)
    {
        fprintf(stderr, "Failed to allocate memory for rule\n");
        exit(EXIT_FAILURE);
    }

    return ret;
}

/*
 Sprawdza czy znak jest cyfrą dzisiętną.
 Potrzebne, bo iswdigit zależnie od locale może uznawać
 inne znaki niż dziesiętne.
 */
bool is_decimal(wchar_t wc)
{
    wchar_t digits[] = {
        L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9'
    };

    for (size_t i = 0; i < 10; i++) if (wc == digits[i]) return true;

    return false;
}

/*
 Konwertuje znak na liczbę
 */
int decimal_to_int(wchar_t wc)
{
    return wc - L'0';
}

/*
 Zlicza liczbę (unikalnych) zmiennych występujących tylko
 po prawej stronie reguły.
 */
int vars_only_in_right(Rule *rule)
{
    bool var_present_in_left[10];
    bool var_present_in_right[10];

    for (size_t i = 0; i < 10; i++)
    {
        var_present_in_left[i] = false;
        var_present_in_right[i] = false;
    }

    for (size_t i = 0; i < rule->left_len; i++)
    {
        if (is_decimal(rule->left[i]))
        {
            var_present_in_left[decimal_to_int(rule->left[i])] = true;
        }
    }

    for (size_t i = 0; i < rule->right_len; i++)
    {
        if (is_decimal(rule->right[i]))
        {
            var_present_in_right[decimal_to_int(rule->right[i])] = true;
        }
    }

    int only_in_right = 0;
    for (size_t i = 0; i < 10; i++)
    {
        if (!var_present_in_left[i]
            && var_present_in_right[i])
        {
            only_in_right++;
        }
    }

    return only_in_right;
}

/**@}*/
/** @name Elementy interfejsu
  @{
  */

Rule * rule_new(const wchar_t *left, const wchar_t *right,
                int cost, enum rule_flag flag)
{
    Rule *rule = (Rule *) emalloc(sizeof(Rule));

    rule->left_len = wcslen(left);
    rule->right_len = wcslen(right);

    rule->left = emalloc(sizeof(wchar_t) * (rule->left_len + 1));
    rule->right = emalloc(sizeof(wchar_t) * (rule->right_len +1 ));

    wcscpy(rule->left, left);
    wcscpy(rule->right, right);
    rule->cost = cost;
    rule->flag = flag;

    return rule;
}

void rule_done(Rule *rule) {
    free(rule->left);
    free(rule->right);
    free(rule);
}

bool rule_is_legal(Rule *rule)
{
    if (vars_only_in_right(rule) > 1)
    {
        return false;
    }

    if (rule->left_len == 0 && rule->right_len == 0 && rule->flag != RULE_SPLIT)
    {
        return false;
    }

    return true;
}

int rule_save(const Rule *rule, IO *io)
{
    if (io_printf(io, L"%ls*", rule->left) < 0) return -1;
    if (io_printf(io, L"%ls*", rule->right) < 0) return -1;
    if (io_printf(io, L"%d*", rule->cost) < 0) return -1;
    if (io_printf(io, L"%d\n", rule->flag) < 0) return -1;

    return 0;
}

/**@}*/