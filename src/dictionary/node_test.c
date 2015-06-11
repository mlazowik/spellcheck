/** @file
    Testy węzła.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-06-11
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cmocka.h>
#include <locale.h>
#include "node.c"
#include "utils.h"

#define _GNU_SOURCE

/**
  Testuje inicjalizację węzła.
  @param state Środowisko testowe.
  */
static void node_init_test(void** state)
{
    Node *node = node_new(L'ą');

    assert_true(node_get_key(node) == L'ą');

    node_done(node);
}

/**
  Testuje ustawianie czy w wężle kończy się słowo.
  @param state Środowisko testowe.
  */
static void node_is_word_test(void** state)
{
    Node *node = node_new(L'ą');

    assert_true(node_get_key(node) == L'ą');
    assert_false(node_is_word(node));
    node_set_is_word(node, true);
    assert_true(node_is_word(node));
    node_set_is_word(node, false);
    assert_false(node_is_word(node));

    node_done(node);
}

/**
  Testuje dodawanie syna.
  @param state Środowisko testowe.
  */
static void node_add_child_test(void** state)
{
    Node *node = node_new(L'ą');

    node_add_child(node, L'ź');
    node_add_child(node, L'ź');
    assert_int_equal(node_children_count(node), 1);

    node_done(node);
}

/**
  Testuje branie syna.
  @param state Środowisko testowe.
  */
static void node_get_child_test(void** state)
{
    Node *node = node_new(L'ą');

    node_add_child(node, L'ź');
    assert_non_null(node_get_key(node_get_child(node, L'ź')));
    assert_true(node_get_key(node_get_child(node, L'ź')) == L'ź');

    node_done(node);
}

/**
  Przygotowsuje środowisko testowe
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p.
  */
static int node_setup(void **state)
{
    Node *node = node_new(L'ą');

    size_t n_letters = 5;
    wchar_t letters[] = {L'ą', L'b', L'ź', L'ć', L'x'};

    for (size_t i = 0; i < n_letters; i++)
    {
        node_add_child(node, letters[i]);
        Node *child = node_get_child(node, letters[i]);
        if (letters[i] != L'x') node_set_is_word(child, true);

        for (size_t j = 0; j < n_letters; j++)
        {
            node_add_child(child, letters[j]);

            // słowami są tylko ciągi o różnych znakach
            if (letters[i] != letters[j])
            {
                node_set_is_word(node_get_child(child, letters[j]), true);
            }
        }
    }

    *state = node;

    return 0;
}

/**
  Niszczy środowisko testwowe.
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p
  */
static int node_teardown(void **state)
{
    Node *node = *state;

    node_done(node);

    return 0;
}

/**
  Testuje branie ojca.
  @param state Środowisko testowe.
  */
static void node_get_parent_test(void** state)
{
    node_setup(state);

    Node *node = *state;
    assert_ptr_equal(node, node_get_parent(node_get_child(node, L'ć')));

    node_teardown(state);
}

/**
  Testuje usuwanie syna.
  @param state Środowisko testowe.
  */
static void node_remove_child_test(void** state)
{
    node_setup(state);

    Node *node = *state;
    node_remove_child(node, L'ć');
    assert_null(node_get_child(node, L'ć'));
    assert_null(node_get_child(node, L'ł'));
    assert_non_null(node_get_child(node, L'ą'));
    assert_int_equal(node_children_count(node), 4);

    node_teardown(state);
}

/**
  Testuje wyszukiwanie słowa.
  @param state Środowisko testowe.
  */
static void node_has_word_test(void** state)
{
    node_setup(state);

    Node *node = *state;
    assert_true(node_has_word(node, L"ą"));
    assert_false(node_has_word(node, L"x"));
    assert_false(node_has_word(node, L"ł"));

    assert_true(node_has_word(node, L"xą"));
    assert_true(node_has_word(node, L"ąx"));
    assert_false(node_has_word(node, L"xx"));
    assert_false(node_has_word(node, L"źł"));

    node_teardown(state);
}

/**
  Testuje wyszukiwanie podpowiedzi.
  @param state Środowisko testowe.
  */
static void node_get_hints_test(void** state)
{
    node_setup(state);

    Node *node = *state;
    Trie *hints = trie_new();

    node_get_hints(node, L"xą", hints);

    assert_true(trie_has_word(hints, L"xą"));

    assert_true(trie_has_word(hints, L"ą"));
    assert_false(trie_has_word(hints, L"x"));

    assert_true(trie_has_word(hints, L"xć"));
    assert_true(trie_has_word(hints, L"ćą"));

    trie_done(hints);
    hints = trie_new();

    node_get_hints(node, L"x", hints);

    assert_true(trie_has_word(hints, L"ć"));
    assert_true(trie_has_word(hints, L"xć"));
    assert_false(trie_has_word(hints, L"x"));
    assert_false(trie_has_word(hints, L"ćą"));

    trie_done(hints);
    node_teardown(state);
}

/**
  Testuje zapisanie słów do listy słów.
  @param state Środowisko testowe.
  */
static void node_add_words_to_list_test(void** state)
{
    node_setup(state);

    Node *node = *state;
    wchar_t prefix[100];
    struct word_list list, reference;
    word_list_init(&list);
    word_list_init(&reference);

    size_t n_letters = 5;
    wchar_t letters[] = {L'ą', L'b', L'ź', L'ć', L'x'};

    for (size_t i = 0; i < 3; i++) prefix[i] = L'ł';

    for (size_t i = 0; i < n_letters; i++)
    {
        prefix[3] = letters[i];
        prefix[4] = L'\0';
        if (letters[i] != L'x') word_list_add(&reference, prefix);

        prefix[5] = L'\0';
        for (size_t j = 0; j < n_letters; j++)
        {
            prefix[4] = letters[j];

            // słowami są tylko ciągi o różnych znakach
            if (letters[i] != letters[j])
            {
                word_list_add(&reference, prefix);
            }
        }
    }

    node_add_words_to_list(node, prefix, 3, &list);

    word_list_sort(&list);
    word_list_sort(&reference);

    assert_int_equal(word_list_size(&list), word_list_size(&reference));

    for (size_t i = 0; i < word_list_size(&list); i++)
    {
        assert_true(wcscmp(word_list_get(&list)[i], word_list_get(&reference)[i]) == 0);
    }

    word_list_done(&list);
    word_list_done(&reference);
    node_teardown(state);
}

/**
  Testuje zapisywanie poddrzewa węzła.
  @param state Środowisko testowe.
  */
static void node_save_test(void** state)
{
    node_setup(state);

    Node *node = *state;
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

    assert_true(node_save(node, io) == 0);

    fclose(stream);

    assert_true(wcscmp(L"b*b^x*^ą*^ć*^ź*^^xb*^x^ą*^ć*^ź*^^ą*b*^x*^ą^ć*^ź*^^ć*b*^x*^ą*^ć^ź*^^ź*b*^x*^ą*^ć*^ź^^", buf) == 0);

    io_done(io);
#   undef free
    free(buf);
#   define free(ptr) _test_free(ptr, __FILE__, __LINE__)
    node_teardown(state);
}

/**
  Główna funkcja uruchamiająca testy.
  */
int main(void)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");

    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(node_init_test),
        cmocka_unit_test(node_is_word_test),
        cmocka_unit_test(node_add_child_test),
        cmocka_unit_test(node_get_child_test),
        cmocka_unit_test(node_get_parent_test),
        cmocka_unit_test(node_remove_child_test),
        cmocka_unit_test(node_has_word_test),
        cmocka_unit_test(node_get_hints_test),
        cmocka_unit_test(node_add_words_to_list_test),
        cmocka_unit_test(node_save_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
