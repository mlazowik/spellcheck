/** @file
    Implementacja biblioteki obsługującej wejście i wyjście.

    @ingroup io
    @author Michał Łazowik <m.lazowik@student.uw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-06-06
 */

#include "io.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/**
  Struktura przechowująca we/wy.
 */
struct io
{
    /// Wejście
    FILE *in;
    /// Wyjście
    FILE *out;
    /// Wyjście błędów
    FILE *err;

    /// Numer znaku w linii
    size_t n_char;
    /// Numer linii
    size_t n_line;
};

/** @name Elementy interfejsu
  @{
  */

IO * io_new(FILE *in, FILE *out, FILE *err)
{
    IO *io = (IO *) malloc(sizeof(IO));

    io->in = in;
    io->out = out;
    io->err = err;

    io->n_char = 1;
    io->n_line = 1;

    return io;
}

void io_done(IO *io)
{
    free(io);
}

wint_t io_get_next(IO *io)
{
    wint_t c = fgetwc(io->in);

    io->n_char = io->n_char + 1;
    if (c == L'\n')
    {
        io->n_char = 1;
        io->n_line = io->n_line + 1;
    }

    if (ferror(io->in))
    {
        io_eprintf(io, L"Failed to read\n");
    }

    return c;
}

wint_t io_peek_next(IO *io)
{
    wint_t c = fgetwc(io->in);

    if (ferror(io->in))
    {
        io_eprintf(io, L"Failed to read\n");
    }

    if (c != WEOF) ungetwc(c, io->in);

    return c;
}

int io_printf(IO *io, const wchar_t *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    int ret = vfwprintf(io->out, fmt, args);
    va_end(args);

    return ret;
}

int io_eprintf(IO *io, const wchar_t *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    int ret = vfwprintf(io->err, fmt, args);
    va_end(args);

    return ret;
}

size_t io_get_n_char(IO *io)
{
    return io->n_char;
}

size_t io_get_n_line(IO *io)
{
    return io->n_line;
}

/**@}*/
