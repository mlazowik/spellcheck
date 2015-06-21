/** @file
    Testy reguły.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-06-21
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cmocka.h>
#include <locale.h>
#include "rule.c"
#include "utils.h"

/// Rozszerzenia GNU
#define _GNU_SOURCE

/** @def free(ptr)
    Podminia free na _test_free z cmocka
  */

/**
  Testuje inicjalizację reguły.
  @param state Środowisko testowe.
  */
static void rule_init_test(void** state)
{
    Rule *rule = rule_new(L"ąbc", L"dęf",
                          2, RULE_NORMAL);

    rule_done(rule);
}

/**
  Testuje sprawdzanie zgodności reguły z założeniami.
  @param state Środowisko testowe.
  */
static void rule_is_legal_test(void** state)
{
    Rule *normal = rule_new
    (
        L"ąb1c1",
        L"1d112ęf2",
        2,
        RULE_NORMAL
    );

    Rule *empty_split = rule_new(L"", L"", 1, RULE_SPLIT);

    Rule *too_many_free_vars = rule_new
    (
        L"ąb1c21",
        L"1d443112ęf2",
        2,
        RULE_NORMAL
    );

    Rule *empty_non_split = rule_new(L"", L"", 10, RULE_END);

    assert_true(rule_is_legal(normal));
    assert_true(rule_is_legal(empty_split));
    assert_false(rule_is_legal(too_many_free_vars));
    assert_false(rule_is_legal(empty_non_split));

    rule_done(normal);
    rule_done(empty_split);
    rule_done(too_many_free_vars);
    rule_done(empty_non_split);
}

/**
  Testuje zapisywanie reguły.
  @param state Środowisko testowe.
  */
static void rule_save_test(void** state)
{
    Rule *normal = rule_new
    (
        L"ąb1c1",
        L"1d112ęf2",
        2,
        RULE_BEGIN
    );

    FILE *stream;
    wchar_t *buf = NULL;
    size_t len;

    stream = open_wmemstream(&buf, &len);
    if (stream == NULL)
    {
        fprintf(stderr, "Failed to open memory stream\n");
        exit(EXIT_FAILURE);
    }

    IO *io = io_new(stdin, stream, stderr);

    assert_true(rule_save(normal, io) == 0);

    fclose(stream);

    assert_true(wcscmp(L"ąb1c1*1d112ęf2*2*1\n", buf) == 0);

    io_done(io);
#   undef free
    free(buf);
#   define free(ptr) _test_free(ptr, __FILE__, __LINE__)
    rule_done(normal);
}

/**
  Główna funkcja uruchamiająca testy.
  */
int main(void)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");

    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(rule_init_test),
        cmocka_unit_test(rule_is_legal_test),
        cmocka_unit_test(rule_save_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
