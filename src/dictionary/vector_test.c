/** @file
    Testy wektora.

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
#include "vector.c"
#include "utils.h"

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
  Testuje inicjalizację wektora.
  @param state Środowisko testowe.
  */
static void vector_init_test(void** state)
{
    Vector *vector = vector_new(free_int);
    assert_int_equal(vector_size(vector), 0);
    vector_done(vector);
}

/**
  Testuje dodawanie do wektora.
  @param state Środowisko testowe.
  */
static void vector_insert_test(void** state)
{
    Vector *vector = vector_new(free_int);

    int *a = int_new(45);
    int *b = int_new(44);
    int *c = int_new(45);

    vector_insert(vector, 0, c);
    vector_insert(vector, 0, b);
    vector_insert(vector, 1, a);

    assert_int_equal(vector_size(vector), 3);
    assert_ptr_equal(vector_get_by_index(vector, 0), b);
    assert_ptr_equal(vector_get_by_index(vector, 1), a);
    assert_ptr_equal(vector_get_by_index(vector, 2), c);

    free_int(a);
    free_int(b);
    free_int(c);
    vector_done(vector);
}

/**
  Testuje dodawanie na koniec wektora.
  @param state Środowisko testowe.
  */
static void vector_push_back_test(void** state)
{
    Vector *vector = vector_new(free_int);

    int *a = int_new(45);
    int *b = int_new(44);
    int *c = int_new(45);

    vector_push_back(vector, a);
    vector_push_back(vector, b);
    vector_push_back(vector, c);

    assert_int_equal(vector_size(vector), 3);
    assert_ptr_equal(vector_get_by_index(vector, 0), a);
    assert_ptr_equal(vector_get_by_index(vector, 1), b);
    assert_ptr_equal(vector_get_by_index(vector, 2), c);

    free_int(a);
    free_int(b);
    free_int(c);
    vector_done(vector);
}

/**
  Testuje automatyczne rozszerzanie i zmniejszanie się wektora.
  @param state Środowisko testowe.
  */
static void vector_auto_resize_test(void** state)
{
    Vector *vector = vector_new(free_int);
    int *numbers[MINIMAL_CAPACITY + 5];

    for (size_t i = 0; i < MINIMAL_CAPACITY + 5; i++)
    {
        numbers[i] = int_new(i);
        vector_push_back(vector, numbers[i]);
    }

    assert_int_equal(vector_size(vector), MINIMAL_CAPACITY + 5);

    for (size_t i = 0; i < MINIMAL_CAPACITY + 5; i++)
    {
        assert_ptr_equal(vector_get_by_index(vector, i), numbers[i]);
    }

    for (size_t i = 0; i < MINIMAL_CAPACITY + 5; i++)
    {
        vector_pop_back(vector);
    }

    assert_int_equal(vector->capacity, MINIMAL_CAPACITY);

    vector_done(vector);
}

/**
  Przygotowsuje środowisko testowe
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p.
  */
static int vector_setup(void **state)
{
    int *numbers[] =
    {
        int_new(10),
        int_new(20),
        int_new(30),
        int_new(40),
    };

    Vector *vector = vector_new(free_int);

    for (size_t i = 0; i < 4; i++)
    {
        vector_push_back(vector, numbers[i]);
    }

    *state = vector;

    return 0;
}

/**
  Niszczy środowisko testwowe.
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p
  */
static int vector_teardown(void **state)
{
    Vector *vector = *state;

    vector_clear(vector);
    vector_done(vector);

    return 0;
}

/**
  Testuje usuwanie elementu z wektora.
  @param state Środowisko testowe.
  */
static void vector_delete_test(void** state)
{
    vector_setup(state);
    Vector *vector = *state;

    vector_delete(vector, 1);
    assert_int_equal(vector_size(vector), 3);
    assert_int_equal(*(int*)vector_get_by_index(vector, 1), 30);

    vector_teardown(state);
}

/**
  Testuje usuwanie końcowego elementu z wektora.
  @param state Środowisko testowe.
  */
static void vector_pop_back_test(void** state)
{
    vector_setup(state);
    Vector *vector = *state;

    vector_pop_back(vector);
    vector_pop_back(vector);
    assert_int_equal(vector_size(vector), 2);
    assert_int_equal(*(int*)vector_get_by_index(vector, 0), 10);
    assert_int_equal(*(int*)vector_get_by_index(vector, 1), 20);

    vector_teardown(state);
}

/**
  Testuje pobieranie elementu z wektora wg jego indeksu.
  @param state Środowisko testowe.
  */
static void vector_get_by_index_test(void** state)
{
    vector_setup(state);
    Vector *vector = *state;

    assert_true(*(int*)(vector_get_by_index(vector, 1)) == 20);
    assert_null(vector_get_by_index(vector, 4));
    assert_null(vector_get_by_index(vector, 1000));
    assert_null(vector_get_by_index(vector, -1));

    vector_teardown(state);
}

/**
  Główna funkcja uruchamiająca testy.
  */
int main(void)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");

    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(vector_init_test),
        cmocka_unit_test(vector_insert_test),
        cmocka_unit_test(vector_push_back_test),
        cmocka_unit_test(vector_auto_resize_test),
        cmocka_unit_test(vector_delete_test),
        cmocka_unit_test(vector_pop_back_test),
        cmocka_unit_test(vector_get_by_index_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
