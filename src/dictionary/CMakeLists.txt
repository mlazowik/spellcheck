# dodajemy bibliotekę dictionary, stworzoną na podstawie pliku dictionary.c
# biblioteka będzie dołączana statycznie (czyli przez linkowanie pliku .o)

add_library (dictionary dictionary.c dictionary.h word_list.c word_list.h
	map.c map.h node.c node.h trie.h trie.c)

# przy kompilacji programu należy dołączyć bibliotekę
target_link_libraries (dictionary io)

if (CMOCKA)
    add_definitions (-DUNIT_TESTING)

    # dodajemy pliki wykonywalne z testami
    add_executable (word_list_test word_list_test.c)
    add_executable (map_test map_test.c)

    # i linkujemy je z biblioteką do testowania
    target_link_libraries (word_list_test ${CMOCKA})
	target_link_libraries (map_test dictionary ${CMOCKA})

    # wreszcie deklarujemy, że są to testy
	add_test (word_list_unit_test word_list_test)
    add_test (map_unit_test map_test)
endif (CMOCKA)
