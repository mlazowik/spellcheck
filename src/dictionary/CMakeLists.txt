# dodajemy bibliotekę dictionary, stworzoną na podstawie pliku dictionary.c
# biblioteka będzie dołączana statycznie (czyli przez linkowanie pliku .o)

add_library (dictionary dictionary.c dictionary.h word_list.c word_list.h
	map.c map.h node.c node.h trie.h trie.c)

# przy kompilacji programu należy dołączyć bibliotekę
target_link_libraries (dictionary io)

if (CMOCKA)
    # dodajemy plik wykonywalny z testem    
    add_executable (word_list_test word_list.c word_list_test.c)

    # i linkujemy go z biblioteką do testowania
    target_link_libraries (word_list_test ${CMOCKA})

    # wreszcie deklarujemy, że to test
    add_test (word_list_unit_test word_list_test)
endif (CMOCKA)