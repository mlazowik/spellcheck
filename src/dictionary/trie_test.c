/** @file
    Testy drzewa trie.

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
#include "trie.c"
#include "utils.h"

/**
  Testuje inicjalizację drzewa.
  @param state Środowisko testowe.
  */
static void trie_init_test(void** state)
{
    Trie *trie = trie_new();

    trie_done(trie);
}

/**
  Testuje wstawianie do drzewa.
  @param state Środowisko testowe.
  */
static void trie_insert_word_test(void** state)
{
    Trie *trie = trie_new();

    size_t n_words = 4;
    wchar_t *words[] = {L"wątły", L"wątły", L"wątlejszy", L"łódka"};

    for (size_t i = 0; i < n_words; i++)
    {
        if (i == 1) assert_false(trie_insert_word(trie, words[i]));
        else assert_true(trie_insert_word(trie, words[i]));
    }

    for (size_t i = 0; i < n_words; i++) assert_true(trie_has_word(trie, words[i]));

    trie_done(trie);
}

/**
  Przygotowsuje środowisko testowe
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p.
  */
static int trie_setup(void **state)
{
    Trie *trie = trie_new();

    size_t n_words = 3;
    wchar_t *words[] = {L"wątły", L"wątlejszy", L"łódka"};

    for (size_t i = 0; i < n_words; i++) trie_insert_word(trie, words[i]);

    *state = trie;

    return 0;
}

/**
  Niszczy środowisko testwowe.
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p
  */
static int trie_teardown(void **state)
{
    Trie *trie = *state;

    trie_done(trie);

    return 0;
}

/**
  Testuje usuwanie z drzewa.
  @param state Środowisko testowe.
  */
static void trie_delete_word_test(void** state)
{
    trie_setup(state);

    Trie *trie = *state;

    assert_false(trie_delete_word(trie, L"wąt"));

    assert_true(trie_delete_word(trie, L"łódka"));
    assert_false(trie_delete_word(trie, L"łódka"));
    assert_false(trie_has_word(trie, L"łódka"));

    assert_true(trie_delete_word(trie, L"wątlejszy"));
    assert_false(trie_has_word(trie, L"wątlejszy"));
    assert_true(trie_has_word(trie, L"wątły"));

    trie_teardown(state);
}

/**
  Testuje podpowiedzi.
  @param state Środowisko testowe.
  */
static void trie_get_hints_test(void** state)
{
    trie_setup(state);

    Trie *trie = *state;

    Trie *hints = trie_new();
    trie_get_hints(trie, L"wątly", hints);
    assert_true(trie_has_word(hints, L"wątły"));

    trie_done(hints);
    trie_teardown(state);
}

/**
  Testuje zapisyanie do listy słów.
  @param state Środowisko testowe.
  */
static void trie_to_word_list_test(void** state)
{
    trie_setup(state);

    Trie *trie = *state;

    struct word_list list;
    word_list_init(&list);

    trie_to_word_list(trie, &list);

    assert_int_equal(word_list_size(&list), 3);
    for (size_t i = 0; i < word_list_size(&list); i++)
    {
        assert_true(trie_has_word(trie, word_list_get(&list)[i]));
    }

    word_list_done(&list);
    trie_teardown(state);
}

/**
  Główna funkcja uruchamiająca testy.
  */
int main(void)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");

    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(trie_init_test),
        cmocka_unit_test(trie_insert_word_test),
        cmocka_unit_test(trie_delete_word_test),
        cmocka_unit_test(trie_get_hints_test),
        cmocka_unit_test(trie_to_word_list_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
