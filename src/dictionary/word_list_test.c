/** @file
    Testy listy słów.

    @ingroup dictionary
    @author Tomasz Kociumaka <kociumaka@mimuw.edu.pl>>
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-06-08
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cmocka.h>
#include <locale.h>
#include "word_list.c"
#include "utils.h"

/**
  Napis testowy.
  */
const wchar_t* test   = L"Test string";
/**
  Napis testowy.
  */
const wchar_t* first  = L"First string";
/**
  Napis testowy.
  */
const wchar_t* second = L"Second string";
/**
  Napis testowy.
  */
const wchar_t* third  = L"Third string";

/**
  Testuje inicjalizację listy.
  @param state Środowisko testowe.
  */
static void word_list_init_test(void** state)
{
    struct word_list l;
    word_list_init(&l);
    assert_int_equal(word_list_size(&l), 0);
    word_list_done(&l);
}

/**
  Testuje dodawanie słowa do listy.
  @param state Środowisko testowe.
  */
static void word_list_add_test(void** state)
{
    struct word_list l;
    word_list_init(&l);
    word_list_add(&l, test);
    assert_int_equal(word_list_size(&l), 1);
    assert_true(wcscmp(test, word_list_get(&l)[0]) == 0);
    word_list_done(&l);
}

/**
  Przygotowsuje środowisko testowe
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p.
  */
static int word_list_setup(void **state)
{
    struct word_list *l = malloc(sizeof(struct word_list));
    if (!l)
        return -1;
    word_list_init(l);
    word_list_add(l, first);
    word_list_add(l, second);
    word_list_add(l, third);
    *state = l;
    return 0;
}

/**
  Niszczy środowisko testwowe.
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p
  */
static int word_list_teardown(void **state)
{
    struct word_list *l = *state;
    word_list_done(l);
    free(l);
    return 0;
}

/**
  Testuje pobieranie listy słów.
  @param state Środowisko testowe.
  */
static void word_list_get_test(void** state)
{
    word_list_setup(state);

    struct word_list *l = *state;
    assert_true(wcscmp(first, word_list_get(l)[0]) == 0);
    assert_true(wcscmp(second, word_list_get(l)[1]) == 0);
    assert_true(wcscmp(third, word_list_get(l)[2]) == 0);

    word_list_teardown(state);
}

/**
  Testuje wielokrotne włożenie tego samego słowa.
  @param state Środowisko testowe.
  */
static void word_list_repeat_test(void** state)
{
    word_list_setup(state);

    struct word_list *l = *state;
    word_list_add(l, third);
    assert_int_equal(word_list_size(l), 4);
    assert_true(wcscmp(third, word_list_get(l)[3]) == 0);

    word_list_teardown(state);
}

/**
  Testuje sortowanie listy
  @param state Środowisko testowe.
  */
static void word_list_sort_test(void **state)
{
    wchar_t *words[] = {L"felin", L"fen", L"fin", L"féin", L"mein", L"tein"};
    wchar_t *ordered[] = {L"féin", L"felin", L"fen", L"fin", L"mein", L"tein"};
    struct word_list list;

    word_list_init(&list);

    for (size_t i = 0; i < 6; i++)
    {
        word_list_add(&list, words[i]);
    }

    word_list_sort(&list);

    for (size_t i = 0; i < 6; i++)
    {
        assert_true(wcscmp(word_list_get(&list)[i], ordered[i]) == 0);
    }

    word_list_done(&list);
}

/**
  Testuje automatyczne zwiększanie pojemności listy.
  Wstawia więcej słów niż początkowa pojemność.
  @param state Środowisko testowe.
  */
static void word_list_auto_resize_test(void **state)
{
    struct word_list l;
    word_list_init(&l);

    for (size_t i = 0; i < MINIMAL_CAPACITY + 3; i++)
    {
        word_list_add(&l, test);
    }

    for (size_t i = 0; i < MINIMAL_CAPACITY + 3; i++)
    {
        assert_true(wcscmp(test, word_list_get(&l)[i]) == 0);
    }

    word_list_done(&l);
}

/**
  Główna funkcja uruchamiająca testy.
  */
int main(void)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");

    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(word_list_init_test),
        cmocka_unit_test(word_list_add_test),
        cmocka_unit_test(word_list_get_test),
        cmocka_unit_test(word_list_repeat_test),
        cmocka_unit_test(word_list_auto_resize_test),
        cmocka_unit_test(word_list_sort_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
