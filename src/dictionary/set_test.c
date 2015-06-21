/** @file
    Testy seta.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-06-20
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cmocka.h>
#include <locale.h>
#include "set.c"
#include "utils.h"

/**
  Porównuje dwie liczby
  @param a Pierwsza liczba
  @param b Druga liczba
  */
static int compare_int(void *a, void *b)
{
    int _a = *(int*)a;
    int _b = *(int*)b;

    if (_a > _b) return 1;
    if (_a == _b) return 0;
    return -1;
}

/**
  Destrukcja inta.
  @param el Wartość do usunięcia
  */
static void free_int(void *el)
{
    free(el);
}

/**
  Tworzenie inta
  @param a Wartość którą ma przyjąć
  */
static int * int_new(int a)
{
    int *ret = (int*) malloc(sizeof(int));
    *ret = a;
    return ret;
}

/**
  Testuje inicjalizację seta.
  @param state Środowisko testowe.
  */
static void set_init_test(void** state)
{
    Set *set = set_new(compare_int, free_int);
    assert_int_equal(set_size(set), 0);
    set_done(set);
}

/**
  Testuje dodawanie do seta.
  @param state Środowisko testowe.
  */
static void set_insert_test(void** state)
{
    Set *set = set_new(compare_int, free_int);

    int *a = int_new(45);
    int *b = int_new(44);

    set_insert(set, a);
    set_insert(set, b);

    assert_int_equal(set_size(set), 2);
    assert_ptr_equal(set_get_by_index(set, 0), b);
    assert_ptr_equal(set_get_by_index(set, 1), a);

    free_int(a);
    free_int(b);
    set_done(set);
}

/**
  Testuje automatyczne rozszerzanie i zmniejszanie się seta.
  @param state Środowisko testowe.
  */
static void set_auto_resize_test(void** state)
{
    const size_t big_size = 20;

    Set *set = set_new(compare_int, free_int);
    int *numbers[big_size];

    for (size_t i = 0; i < big_size; i++)
    {
        numbers[i] = int_new(i);
        set_insert(set, numbers[i]);
    }

    assert_int_equal(set_size(set), big_size);

    for (size_t i = 0; i < big_size; i++)
    {
        assert_ptr_equal(set_get_by_index(set, i), numbers[i]);
    }

    for (size_t i = 0; i < big_size; i++)
    {
        set_delete(set, numbers[i]);
    }

    set_done(set);
}

/**
  Testuje wyszukiwanie pozycji dla elementu.
  @param state Środowisko testowe.
  */
static void find_position_test(void** state)
{
    Set *set = set_new(compare_int, free_int);

    int *a = int_new(10);
    int *b = int_new(20);
    int *c = int_new(30);
    int *d = int_new(40);

    set_insert(set, c);
    assert_int_equal(find_position(set, a), 0);
    assert_int_equal(find_position(set, c), 0);
    assert_int_equal(find_position(set, d), 1);

    set_insert(set, a);
    assert_int_equal(find_position(set, a), 0);
    assert_int_equal(find_position(set, b), 1);
    assert_int_equal(find_position(set, c), 1);
    assert_int_equal(find_position(set, d), 2);

    free_int(a);
    free_int(b);
    free_int(c);
    free_int(d);

    set_done(set);
}

/**
  Przygotowsuje środowisko testowe
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p.
  */
static int set_setup(void **state)
{
    int *numbers[] =
    {
        int_new(10),
        int_new(20),
        int_new(30),
        int_new(40),
    };

    Set *set = set_new(compare_int, free_int);

    for (size_t i = 0; i < 4; i++)
    {
        set_insert(set, numbers[i]);
    }

    *state = set;

    return 0;
}

/**
  Niszczy środowisko testwowe.
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p
  */
static int set_teardown(void **state)
{
    Set *set = *state;

    for (size_t i = 0; i < set_size(set); i++)
    {
        free_int(set_get_by_index(set, i));
    }

    set_done(set);

    return 0;
}

/**
  Testuje dodawanie istniejącego elementu do seta.
  @param state Środowisko testowe.
  */
static void set_insert_existing_test(void** state)
{
    set_setup(state);
    Set *set = *state;
    int *existing = int_new(20);

    assert_false(set_insert(set, existing));
    assert_int_equal(set_size(set), 4);

    free_int(existing);
    set_teardown(state);
}

/**
  Testuje usuwanie elementu z seta.
  Zarówno istniejącego jak i niestniejącego.
  @param state Środowisko testowe.
  */
static void set_delete_test(void** state)
{
    set_setup(state);
    Set *set = *state;
    int *existing = int_new(20);
    int *non_existing = int_new(25);

    assert_true(set_delete(set, existing));
    assert_int_equal(set_size(set), 3);

    assert_false(set_delete(set, non_existing));

    free_int(existing);
    free_int(non_existing);
    set_teardown(state);
}

/**
  Testuje pobieranie elementu z seta wg jego indeksu.
  Zarówno istniejącego jak i niestniejącego.
  @param state Środowisko testowe.
  */
static void set_get_by_index_test(void** state)
{
    set_setup(state);
    Set *set = *state;

    assert_true(*(int*)(set_get_by_index(set, 1)) == 20);
    assert_null(set_get_by_index(set, 4));
    assert_null(set_get_by_index(set, 1000));
    assert_null(set_get_by_index(set, -1));

    set_teardown(state);
}

/**
  Testuje wyszukiwanie elementu w secie wg klucza.
  Zarówno istniejącego jak i niestniejącego.
  @param state Środowisko testowe.
  */
static void set_find_test(void** state)
{
    set_setup(state);
    Set *set = *state;

    int *existing = int_new(20);
    int *non_existing = int_new(25);

    assert_true(*(int*)set_find(set, existing) == *existing);
    assert_null(set_find(set, non_existing));

    free_int(existing);
    free_int(non_existing);
    set_teardown(state);
}

/**
  Główna funkcja uruchamiająca testy.
  */
int main(void)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");

    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(set_init_test),
        cmocka_unit_test(set_insert_test),
        cmocka_unit_test(set_auto_resize_test),
        cmocka_unit_test(find_position_test),
        cmocka_unit_test(set_insert_existing_test),
        cmocka_unit_test(set_delete_test),
        cmocka_unit_test(set_get_by_index_test),
        cmocka_unit_test(set_find_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
