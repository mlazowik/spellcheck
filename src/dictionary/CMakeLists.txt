# dodajemy bibliotekę dictionary, stworzoną na podstawie pliku dictionary.c
# biblioteka będzie dołączana statycznie (czyli przez linkowanie pliku .o)

add_library (dictionary dictionary.c rule.c word_list.c vector.c set.c node.c trie.c)

# przy kompilacji programu należy dołączyć bibliotekę
target_link_libraries (dictionary io)

if (CMOCKA)
    add_definitions (-DUNIT_TESTING)

    # dodajemy pliki wykonywalne z testami
    add_executable (word_list_test word_list_test.c)
    add_executable (vector_test vector_test.c)
    add_executable (set_test set_test.c)
    add_executable (node_test node_test.c)
    add_executable (trie_test trie_test.c)
    add_executable (rule_test rule_test.c)
    add_executable (dictionary_test dictionary_test.c)

    # i linkujemy je z biblioteką do testowania
    target_link_libraries (word_list_test ${CMOCKA})
    target_link_libraries (vector_test dictionary ${CMOCKA})
    target_link_libraries (set_test dictionary ${CMOCKA})
    target_link_libraries (node_test dictionary ${CMOCKA})
    target_link_libraries (trie_test -Wl,--wrap=io_get_next dictionary ${CMOCKA})
    target_link_libraries (rule_test -Wl,--wrap=io_get_next dictionary ${CMOCKA})
    target_link_libraries (dictionary_test -Wl,--wrap=io_get_next dictionary ${CMOCKA})

    # wreszcie deklarujemy, że są to testy
    add_test (word_list_unit_test word_list_test)
    add_test (vector_unit_test vector_test)
    add_test (set_unit_test set_test)
    add_test (node_unit_test node_test)
    add_test (trie_unit_test trie_test)
    add_test (rule_unit_test rule_test)
    add_test (dictionary_unit_test dictionary_test)
endif (CMOCKA)
