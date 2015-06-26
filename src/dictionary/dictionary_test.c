/** @file
    Testy słownika.

    @ingroup dictionary
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-06-22
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cmocka.h>
#include <locale.h>
#include "dictionary.c"
#include "utils.h"

/** @def free(ptr)
    Podminia free na _test_free z cmocka
  */

/**
  Testuje inicjalizację słownika.
  @param state Środowisko testowe.
  */
static void dictionary_init_test(void** state)
{
    struct dictionary *dict = dictionary_new();

    dictionary_done(dict);
}

/**
  Testuje wstawianie do słownika.
  @param state Środowisko testowe.
  */
static void dictionary_insert_test(void** state)
{
    struct dictionary *dict = dictionary_new();

    size_t n_words = 4;
    wchar_t *words[] = {L"wątły", L"wątły", L"wątlejszy", L"łódka"};

    for (size_t i = 0; i < n_words; i++)
    {
        if (i == 1) assert_false(dictionary_insert(dict, words[i]));
        else assert_true(dictionary_insert(dict, words[i]));
    }

    for (size_t i = 0; i < n_words; i++) assert_true(dictionary_find(dict, words[i]));

    dictionary_done(dict);
}

/**
  Przygotowsuje środowisko testowe
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p.
  */
static int dictionary_setup(void **state)
{
    struct dictionary *dict = dictionary_new();

    size_t n_words = 6;
    wchar_t *words[] = {L"felin", L"fen", L"fin", L"féin", L"mein", L"tein"};

    for (size_t i = 0; i < n_words; i++)
    {
        dictionary_insert(dict, words[i]);
    }

    *state = dict;

    return 0;
}

/**
  Niszczy środowisko testwowe.
  @param state Środowisko testowe.
  @return 0 jeśli się udało, -1 w p.p
  */
static int dictionary_teardown(void **state)
{
    struct dictionary *dict = *state;

    dictionary_done(dict);

    return 0;
}

/**
  Testuje wyszukiwanie w słowniku.
  @param state Środowisko testowe.
  */
static void dictionary_find_test(void** state)
{
    dictionary_setup(state);

    struct dictionary *dict = *state;

    assert_true(dictionary_find(dict, L"féin"));
    assert_false(dictionary_find(dict, L"fein"));

    dictionary_teardown(state);
}

/**
  Testuje usuwanie ze słownika.
  @param state Środowisko testowe.
  */
static void dictionary_delete_test(void** state)
{
    dictionary_setup(state);

    struct dictionary *dict = *state;

    assert_true(dictionary_delete(dict, L"féin"));
    assert_false(dictionary_delete(dict, L"féin"));
    assert_false(dictionary_find(dict, L"féin"));

    dictionary_teardown(state);
}

/**
  Testuje zapisywanie słownika.
  @param state Środowisko testowe.
  */
static void dictionary_save_test(void** state)
{
    struct dictionary *dict = dictionary_new();

    FILE *stream;
    wchar_t *buf = NULL;
    size_t len;

    stream = open_wmemstream(&buf, &len);
    if (stream == NULL)
    {
        fprintf(stderr, "Failed to open memory stream\n");
        exit(EXIT_FAILURE);
    }

    dictionary_insert(dict, L"ciupaga");
    assert_true(dictionary_save(dict, stream) == 0);
    fflush(stream);
    assert_true(wcscmp(L"ciupaga*^^^^^^^\n0\n", buf) == 0);
    fseek(stream, 0, SEEK_SET);

    fclose(stream);
#   undef free
    free(buf);
#   define free(ptr) _test_free(ptr, __FILE__, __LINE__)
    dictionary_done(dict);
}

/**
  Atrapa pobierania kolejnego znaku z wejścia.
  */
wint_t __wrap_io_get_next(IO *io)
{
    return mock();
}

/**
  Atrapa podglądania kolejnego znaku z wejścia.
  */
wint_t __wrap_io_peek_next(IO *io)
{
    wint_t tmp[200];
    int j = 0;
    while ((tmp[j++] = __wrap_io_get_next(io)) != WEOF);

    for (int i = 0; i < j; i++) will_return(__wrap_io_get_next, tmp[i]);

    return tmp[0];
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
  @param io We/wy, aby parametry atrapy się zgadzały.
  */
static void pop_remaining_chars()
{
    while (__wrap_io_get_next(NULL) != WEOF);
}

/**
  Testuje wczytywanie drzewa.
  @param state Środowisko testowe.
  */
static void dictionary_load_test(void** state)
{
    struct dictionary *dict = NULL;

    push_word_to_io_mock(L"ciupagą*^^^^^^^\n13\na*b*3*2\n");
    dict = dictionary_load(stdin);
    pop_remaining_chars();
    assert_non_null(dict);
    assert_true(dictionary_find(dict, L"ciupagą"));
    assert_int_equal(dictionary_hints_max_cost(dict, 2), 13);
    dictionary_done(dict);
}

/**
  Główna funkcja uruchamiająca testy.
  */
int main(void)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");

    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(dictionary_init_test),
        cmocka_unit_test(dictionary_insert_test),
        cmocka_unit_test(dictionary_find_test),
        cmocka_unit_test(dictionary_delete_test),
        cmocka_unit_test(dictionary_save_test),
        cmocka_unit_test(dictionary_load_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
