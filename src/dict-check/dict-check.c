/** @defgroup dict-check Moduł dict-check
    Program sprawdza pisownię w danym tekście na podstawie podanego słownika.
  */
/** @file
    Główny plik modułu dict-check
    @ingroup dict-check
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @date 2015-06-05
    @copyright Uniwersytet Warszawski
  */

#include "dictionary.h"
#include "io.h"
#include <stdbool.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#include <wchar.h>

/**
  Maksymalna długość słowa.
  */
#define MAX_WORD_LENGTH 100

/**
  Czy stosować szczegółowy format wyjścia.
  */
static bool verbose;

/**
  Słownik.
  */
struct dictionary *dict;

/** Zamienia słowo na złożone z małych liter.
  @param[in,out] word Modyfikowane słowo.
  @return 0, jeśli słowo nie jest złożone z samych liter, 1 w p.p.
  */
static int make_lowercase(wchar_t *word)
{
    for (wchar_t *w = word; *w; ++w)
        if (!iswalpha(*w))
            return 0;
        else
            *w = towlower(*w);
    return 1;
}

/**
  Wczytuje słownik z pliku o podanej nazwie
  @param[in] filename Nazwa pliku.
  */
static void load_dictionary(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f || !(dict = dictionary_load(f)))
    {
        fprintf(stderr, "Failed to load dictionary from file %s\n", filename);
        exit(1);
    }

    fclose(f);
}

/**
  Przetwarza opcję z argumentów linii poleceń.
  @param[in] option Napis reprezentujący opcję.
  */
static void parse_option(const char *option)
{
    if (strlen(option) == 2 && option[1] == 'v')
    {
        verbose = true;
    }
    else
    {
        fprintf(stderr, "Unrecognized option: %s\n", option);
        exit(EXIT_FAILURE);
    }
}

/**
  Przetwarza nazwę pliku z argumentów linii poleceń.
  @param[in] filename Nazwa pliku
  */
static void parse_filename(const char *filename)
{
    if (dict != NULL)
    {
        fprintf(stderr, "Only one dictionary file can be loaded\n");
        exit(EXIT_FAILURE);
    }

    load_dictionary(filename);
}

/**
  Przetwarza argumenty linii poleceń.

  Należy podać parametry *bez* ścieżki do programu.
  @param[in] argc Liczba parametrów.
  @param[in] argv[] Tablica parametrów.
  */
static void parse_args(int argc, char *argv[])
{
    for (size_t i = 0; i < argc; i++)
    {
        if (argv[i][0] == '-') parse_option(argv[i]);
        else parse_filename(argv[i]);
    }

    if (dict == NULL)
    {
        fprintf(stderr, "No dictionary file provided\n");
        exit(EXIT_FAILURE);
    }
}

/**
  Wczytuje słowo do `word`.
  @param[in,out] io We/wy.
  @param[in,out] word Docelowe słowo.
  */
static void parse_word(IO *io, wchar_t *word)
{
    int i = 0;
    while (io_peek_next(io) != WEOF && iswalpha(io_peek_next(io)))
    {
        if (i > MAX_WORD_LENGTH)
        {
            fprintf(stderr, "Failed to read word: maximum allowed length is 100");
            exit(EXIT_FAILURE);
        }

        word[i++] = io_get_next(io);
    }

    word[i] = '\0';
}

/**
  Wypisuje podpowiedzi dla danego słowa.
  @param[in,out] io We/wy.
  @param[in] word Słowo dla którego szukane są podpowiedzi.
  */
static void print_hints(IO *io, const wchar_t *word)
{
    struct word_list list;
    wchar_t lowercase[wcslen(word)];

    wcscpy(lowercase, word);
    make_lowercase(lowercase);

    dictionary_hints(dict, lowercase, &list);
    const wchar_t * const *a = word_list_get(&list);

    size_t n_line = io_get_n_line(io);
    size_t n_char = io_get_n_char(io) - wcslen(word);
    io_eprintf(io, L"%d,%d %ls: ", n_line, n_char, word);

    for (size_t i = 0; i < word_list_size(&list); i++)
    {
        if (i > 0) io_eprintf(io, L" ");
        io_eprintf(io, L"%ls", a[i]);
    }

    io_eprintf(io, L"\n");

    word_list_done(&list);
}

/**
  Wypisuje słowo ze wskazaniem czy jest ono w słowniku.

  Jeśli użyta została opcja verbose a słowa nie ma w słowniku, wypisuje także
  podpowiedzi dla tego słowa.
  @param[in,out] io We/wy.
  @param[in] word Słowo do wypisania.
  */
static void print_word(IO *io, const wchar_t *word)
{
    wchar_t lowercase[wcslen(word)];

    wcscpy(lowercase, word);
    make_lowercase(lowercase);

    bool word_exists = dictionary_find(dict, lowercase);

    if (!word_exists) io_printf(io, L"#");
    io_printf(io, L"%ls", word);

    if (verbose && !word_exists) print_hints(io, word);
}

/**
  Przetwarza wejście programu.
  @param[in,out] io We/wy.
  */
static void parse_input(IO *io)
{
    wchar_t word[MAX_WORD_LENGTH+1];

    while (io_peek_next(io) != WEOF)
    {
        if (iswalpha(io_peek_next(io)))
        {
            parse_word(io, word);
            print_word(io, word);
        }
        else
        {
            io_printf(io, L"%lc", io_get_next(io));
        }
    }
}

/**
  Funkcja main.
  Główna funkcja programu do sprawdzania pisowni.
 */
int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "pl_PL.UTF-8");

    verbose = false;
    dict = NULL;

    IO *io = io_new(stdin, stdout, stderr);

    parse_args(argc-1, argv+1);

    parse_input(io);

    io_done(io);

    dictionary_done(dict);

    return 0;
}
