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

/// Rozszerzenia GNU
#define _GNU_SOURCE

/** @def free(ptr)
    Podminia free na _test_free z cmocka
  */

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
  Testuje znajowanie słowa w drzewie.
  @param state Środowisko testowe.
  */
static void trie_has_word_test(void** state)
{
    trie_setup(state);

    Trie *trie = *state;

    assert_true(trie_has_word(trie, L"wątły"));
    assert_false(trie_has_word(trie, L"łódź"));

    trie_teardown(state);
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
  Testuje zapisywanie drzewa.
  @param state Środowisko testowe.
  */
static void trie_save_test(void** state)
{
    Trie *trie = trie_new();

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

    assert_true(trie_save(trie, io) == 0);
    fflush(stream);
    assert_true(wcscmp(L"\n", buf) == 0);
    fseek(stream, 0, SEEK_SET);

    trie_insert_word(trie, L"ciupaga");
    assert_true(trie_save(trie, io) == 0);
    fflush(stream);
    assert_true(wcscmp(L"ciupaga*^^^^^^^\n", buf) == 0);
    fseek(stream, 0, SEEK_SET);

    trie_delete_word(trie, L"ciupaga");
    assert_true(trie_save(trie, io) == 0);
    fflush(stream);
    assert_true(wcscmp(L"\n", buf) == 0);

    fclose(stream);
    io_done(io);
#   undef free
    free(buf);
#   define free(ptr) _test_free(ptr, __FILE__, __LINE__)
    trie_done(trie);
}

/**
  Mock for getting next wchar.
  */
wint_t __wrap_io_get_next(IO *io)
{
    return mock();
}

/**
  Wstawia słowo do atrapy wejścia.
  @param word Słowo
  */
static void push_word_to_io_mock(wchar_t *word)
{
    size_t len = wcslen(word);

    for (size_t i = 0; i < len; i++)
    {
        will_return(__wrap_io_get_next, word[i]);
    }

    will_return(__wrap_io_get_next, WEOF);
}

/**
  Usuwa pozostałe znaki z atrapy wejścia.
  */
static void pop_remaining_chars()
{
    while (__wrap_io_get_next(NULL) != WEOF);
}

/**
  Testuje wczytywanie drzewa.
  @param state Środowisko testowe.
  */
static void trie_load_test(void** state)
{
    Trie *trie = NULL;

    IO *io = io_new(stdin, stdout, stderr);

    push_word_to_io_mock(L"");
    trie = trie_load(io);
    assert_non_null(trie);
    trie_done(trie);

    // Poprawny zapis
    push_word_to_io_mock(L"ciupagą*^^^^^^^\n");
    trie = trie_load(io);
    pop_remaining_chars(io);
    assert_true(trie_has_word(trie, L"ciupagą"));
    assert_false(trie_has_word(trie, L"ciupaga"));
    assert_false(trie_has_word(trie, L"ciupag"));
    assert_false(trie_has_word(trie, L"ciupagąą"));
    trie_done(trie);

    // Próba dojścia wyżej niż korzeń
    push_word_to_io_mock(L"a*^^\n");
    trie = trie_load(io);
    pop_remaining_chars(io);
    assert_null(trie);

    // Znaki spoza alfabetu
    push_word_to_io_mock(L"&*^\n");
    trie = trie_load(io);
    pop_remaining_chars();
    assert_null(trie);

    io_done(io);
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
        cmocka_unit_test(trie_has_word_test),
        cmocka_unit_test(trie_delete_word_test),
        cmocka_unit_test(trie_to_word_list_test),
        cmocka_unit_test(trie_save_test),
        cmocka_unit_test(trie_load_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
