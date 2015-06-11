/** @defgroup io Moduł wejścia/wyjścia
    Biblioteka obsługująca wejście i wyjście.
  */
/** @file
    Interfejs biblioteki obsługującej wejście i wyjście.

    @ingroup io
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2015-06-06
 */

#ifndef __IO_H__
#define __IO_H__

#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>
#include <stdarg.h>

/**
  Struktura przechowująca we/wy.
  */
typedef struct io IO;

/**
  Tworzy nowe we/wy.
  @return Nowe we/wy.
  */
IO * io_new();

/**
  Tworzy nowe we/wy.
  @param[in] in Strumień wejścia.
  @param[in] out Strumień wyjścia.
  @param[in] err Strumień wyjścia błędów.
  @return Nowe we/wy.
  */
IO * io_new(FILE *in, FILE *out, FILE *err);

/**
  Destrukcja we/wy.
  @param[in,out] io We/wy.
  */
void io_done(IO *io);

/**
  Zwraca następny znak z wejścia.
  @param[in,out] io We/wy.
  */
wchar_t io_get_next(IO *io);

/**
  Zwraca następny znak z wejścia, ale go z niego nie zdejmuje.
  @param[in] io We/wy.
  */
wchar_t io_peek_next(IO *io);

/**
  Wypisuje na wyjście.
  @param[in] io We/wy.
  @param[in] fmt Format wyjścia.
  @return <0 jeśli się nie udało
  */
int io_printf(IO *io, const wchar_t *fmt, ...);

/**
  Wypisuje na wyjście błędów.
  @param[in] io We/wy.
  @param[in] fmt Format wyjścia.
  @return <0 jeśli się nie udało
  */
int io_eprintf(IO *io, const wchar_t *fmt, ...);

/**
  Zwraca pozycję "kursora" w linii
  @param[in] io We/wy.
  */
size_t io_get_n_char(IO *io);

/**
  Zwraca numer linii w której znajduje się "kursor".
  @param[in] io We/wy.
  */
size_t io_get_n_line(IO *io);

#endif /* __IO_H__ */
