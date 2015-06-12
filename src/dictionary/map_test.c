/** @file
    Testy mapy.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-06-09
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cmocka.h>
#include <locale.h>
#include "map.c"
#include "utils.h"

/**
  Testuje inicjalizację mapy.
  @param state Środowisko testowe.
  */
static void map_init_test(void** state)
{
    Map *map = map_new();
    assert_int_equal(map_size(map), 0);
    map_done(map);
}

/**
  Testuje dodawanie do mapy.
  @param state Środowisko testowe.
  */
static void map_insert_test(void** state)
{
    Node *node1 = node_new(L'ź');
    Node *node2 = node_new(L'ą');
    Map *map = map_new();

    map_insert(map, L'ź', node1);
    map_insert(map, L'ą', node2);

    assert_int_equal(map_size(map), 2);
    assert_ptr_equal(map_get_by_index(map, 0), node2);
    assert_ptr_equal(map_get_by_index(map, 1), node1);

    node_done(node1);
    node_done(node2);
    map_done(map);
}

/**
  Testuje automatyczne rozszerzanie i zmniejszanie się mapy.
  @param state Środowisko testowe.
  */
static void map_auto_resize_test(void** state)
{
    Map *map = map_new();
    Node *nodes[MINIMAL_CAPACITY + 5];

    for (size_t i = 0; i < MINIMAL_CAPACITY + 5; i++)
    {
        nodes[i] = node_new(L'ą' + 1);
        map_insert(map, L'a' + i, nodes[i]);
    }

    for (size_t i = 0; i < MINIMAL_CAPACITY + 5; i++)
    {
        assert_ptr_equal(map_get_by_index(map, i), nodes[i]);
    }

    assert_int_equal(map_size(map), MINIMAL_CAPACITY + 5);

    for (size_t i = 0; i < MINIMAL_CAPACITY + 5; i++)
    {
        map_delete(map, L'a' + i);
    }

    assert_int_equal(map->capacity, MINIMAL_CAPACITY);

    map_done(map);
}

/**
  Testuje wyszukiwanie pozycji dla znaku.
  @param state Środowisko testowe.
  */
static void find_position_test(void** state)
{
    Map *map = map_new();

    map_insert(map, L'ą', NULL);
    assert_int_equal(find_position(map, L'a'), 0);
    assert_int_equal(find_position(map, L'ą'), 0);
    assert_int_equal(find_position(map, L'ć'), 1);

    map_insert(map, L'a', NULL);
    assert_int_equal(find_position(map, L'a'), 0);
    assert_int_equal(find_position(map, L'b'), 1);
    assert_int_equal(find_position(map, L'ą'), 1);
    assert_int_equal(find_position(map, L'ć'), 2);

    map_done(map);
}

/**
  Przygotowsuje środowisko testowe
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p.
  */
static int map_setup(void **state)
{
    Node *nodes[] =
    {
        node_new(L'a'),
        node_new(L'ą'),
        node_new(L'ź'),
        node_new(L'b'),
    };

    Map *map = map_new();

    for (size_t i = 0; i < 4; i++)
    {
        map_insert(map, node_get_key(nodes[i]), nodes[i]);
    }

    *state = map;

    return 0;
}

/**
  Niszczy środowisko testwowe.
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p
  */
static int map_teardown(void **state)
{
    Map *map = *state;

    for (size_t i = 0; i < map_size(map); i++)
    {
        node_done(map_get_by_index(map, i));
    }

    map_done(map);

    return 0;
}

/**
  Testuje dodawanie istniejącego elementu do mapy.
  @param state Środowisko testowe.
  */
static void map_insert_existing_test(void** state)
{
    map_setup(state);
    Map *map = *state;
    Node *node = node_new(L'ą');

    assert_false(map_insert(map, L'ą', node));
    assert_int_equal(map_size(map), 4);

    node_done(node);
    map_teardown(state);
}

/**
  Testuje usuwanie elementu z mapy.
  Zarówno istniejącego jak i niestniejącego.
  @param state Środowisko testowe.
  */
static void map_delete_test(void** state)
{
    map_setup(state);
    Map *map = *state;

    map_delete(map, L'b');
    assert_int_equal(map_size(map), 3);

    // Usuwanie nieistniejącego.
    assert_false(map_delete(map, L'b'));

    map_teardown(state);
}

/**
  Testuje pobieranie elementu z mapy wg jego indeksu.
  Zarówno istniejącego jak i niestniejącego.
  @param state Środowisko testowe.
  */
static void map_get_by_index_test(void** state)
{
    map_setup(state);
    Map *map = *state;

    assert_true(node_get_key(map_get_by_index(map, 1)) == L'b');
    assert_null(map_get_by_index(map, 4));
    assert_null(map_get_by_index(map, 1000));
    assert_null(map_get_by_index(map, -1));

    map_teardown(state);
}

/**
  Testuje wyszukiwanie elementu w mapie wg klucza.
  Zarówno istniejącego jak i niestniejącego.
  @param state Środowisko testowe.
  */
static void map_find_test(void** state)
{
    map_setup(state);
    Map *map = *state;

    assert_true(node_get_key(map_find(map, L'b')) == L'b');
    assert_true(node_get_key(map_find(map, L'ź')) == L'ź');
    assert_null(map_find(map, L'ó'));

    map_teardown(state);
}

/**
  Główna funkcja uruchamiająca testy.
  */
int main(void)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");

    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(map_init_test),
        cmocka_unit_test(map_insert_test),
        cmocka_unit_test(map_auto_resize_test),
        cmocka_unit_test(find_position_test),
        cmocka_unit_test(map_insert_existing_test),
        cmocka_unit_test(map_delete_test),
        cmocka_unit_test(map_get_by_index_test),
        cmocka_unit_test(map_find_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
