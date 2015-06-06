# dodajemy bibliotekę io, stworzoną na podstawie pliku io.c
# biblioteka będzie dołączana statycznie (czyli przez linkowanie pliku .o)

add_library (io io.c io.h)
