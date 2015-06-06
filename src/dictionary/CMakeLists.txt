# dodajemy bibliotekę dictionary, stworzoną na podstawie pliku dictionary.c
# biblioteka będzie dołączana statycznie (czyli przez linkowanie pliku .o)

add_library (dictionary dictionary.c dictionary.h word_list.c word_list.h
	map.c map.h node.c node.h trie.h trie.c)

# przy kompilacji programu należy dołączyć bibliotekę
target_link_libraries (dictionary io)
