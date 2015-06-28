#include <gtk/gtk.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <argz.h>
#include <iconv.h>
#include <wctype.h>
#include <string.h>
#include "editor.h"
#include "word_list.h"
#include "dictionary.h"

enum CustomResponseType {
  CUSTOM_RESPONSE_ADD
};

static struct dictionary *dict = NULL;
static char *lang = NULL;

static bool select_lang ();

static void delete_dictionary () {
  if (dict != NULL) dictionary_done(dict);
  g_free(lang);
}

static void swap_dictionary (struct dictionary *new_dict, char *new_lang) {
  delete_dictionary();
  dict = new_dict;
  lang = new_lang;
}

/** Zamienia słowo na złożone z małych liter.
  @param[in,out] word Modyfikowane słowo.
  @return 0, jeśli słowo nie jest złożone z samych liter, 1 w p.p.
 */
int make_lowercase (wchar_t *word) {
    for (wchar_t *w = word; *w; ++w)
        if (!iswalpha(*w))
            return 0;
        else
            *w = towlower(*w);
    return 1;
}

void show_about () {
  GtkWidget *dialog = gtk_about_dialog_new();

  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Text Editor");

  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
     "Text Editor for IPP exercises\n");

  gtk_dialog_run(GTK_DIALOG (dialog));
  gtk_widget_destroy(dialog);
}

void show_help (void) {
  GtkWidget *help_window;
  GtkWidget *label;
  char help[5000];

  help_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW (help_window), "Help - Text Editor");
  gtk_window_set_default_size(GTK_WINDOW(help_window), 300, 300);

  strcpy(help,
         "\nAby podłączyć usługę spell-checkera do programu trzeba:\n\n"
         "Dołączyć ją do menu 'Spell' w menubar.\n\n"
         "Pobrać zawartość bufora tekstu z edytora: całą lub fragment,\n"
         "  zapamiętując pozycję.\n\n");
  strcat(help, "\0");

  label = gtk_label_new(help);

  gtk_container_add(GTK_CONTAINER(help_window), label);

  gtk_widget_show_all(help_window);
}

static void error_dialog (const gchar *message) {
  GtkWidget *fail = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                           GTK_BUTTONS_OK,
                                           "%s", message);
  gtk_dialog_run(GTK_DIALOG(fail));
  gtk_widget_destroy(fail);
}

static bool is_on_word (GtkTextIter *end) {
  return (gtk_text_iter_inside_word(end) || gtk_text_iter_ends_word(end));
}

static GtkTextIter get_word_start_iter (GtkTextIter *end) {
  if (!gtk_text_iter_ends_word(end)) gtk_text_iter_forward_word_end(end);
  GtkTextIter start = *end;
  gtk_text_iter_backward_word_start(&start);
  return start;
}

static gunichar * get_word (GtkTextIter *end) {
  GtkTextIter start = get_word_start_iter(end);
  char *word;
  gunichar *wword;

  word = gtk_text_iter_get_text(&start, end);

  wword = g_utf8_to_ucs4_fast(word, -1, NULL);
  g_free(word);

  make_lowercase((wchar_t *)wword);

  return wword;
}

static void highlight (GtkTextIter *end) {
  GtkTextIter start = get_word_start_iter(end);;
  gtk_text_buffer_apply_tag_by_name(editor_buf, "misspelled", &start, end);
}

static void clear_highlight (GtkTextIter *end) {
  GtkTextIter start = get_word_start_iter(end);;
  gtk_text_buffer_remove_tag_by_name(editor_buf, "misspelled", &start, end);
}

static void clear_all_highlighted () {
  GtkTextIter end;

  gtk_text_buffer_get_start_iter(editor_buf, &end);

  bool not_end = true;
  do {
    not_end = gtk_text_iter_forward_word_end(&end);
    if (is_on_word(&end)) clear_highlight(&end);
  } while (not_end);
}

static void check_on_iter (GtkTextIter *end) {
  gunichar *wword;

  if (dict == NULL) {
    if (!select_lang()) {
      error_dialog("Sprawdzanie pisowni nie jest możliwe – nie udało się wybrać języka");
      return;
    };
  }

  if (is_on_word(end)) {
    wword = get_word(end);

    clear_highlight(end);

    if (!dictionary_find(dict, (wchar_t *)wword)) {
      highlight(end);
    }

    g_free(wword);
  }
}

static void check_buffer () {
  GtkTextIter end;

  if (dict == NULL) {
    if (!select_lang()) {
      error_dialog("Sprawdzanie pisowni nie jest możliwe – nie udało się wybrać języka");
      return;
    };
  }

  gtk_text_buffer_get_start_iter(editor_buf, &end);

  bool not_end = true;
  do {
    not_end = gtk_text_iter_forward_word_end(&end);
    check_on_iter(&end);
  } while (not_end);
}

static void check_at_cursor () {
  GtkTextIter end;

  gtk_text_buffer_get_iter_at_mark(editor_buf, &end,
                                   gtk_text_buffer_get_insert(editor_buf));

  check_on_iter(&end);
}

static bool add_lang () {
  bool ret = false;

  GtkWidget *dialog = gtk_dialog_new_with_buttons("Dodawanie języka", NULL,
                                                  0,
                                                  GTK_STOCK_OK,
                                                  GTK_RESPONSE_ACCEPT,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_REJECT,
                                                  NULL);
  GtkWidget *vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

  GtkWidget *label = gtk_label_new("Podaj nazwę języka:");
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 1);

  GtkWidget *input = gtk_entry_new();
  gtk_widget_show(input);
  gtk_box_pack_start(GTK_BOX(vbox), input, FALSE, FALSE, 1);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    char *new_lang = strdup(gtk_entry_get_text((GtkEntry *)input));
    struct dictionary *new_dict = dictionary_new();
    if (dictionary_save_lang(new_dict, new_lang) < 0) {
      error_dialog("Nie udało się stworzyć słownika dla nowego języka");
      dictionary_done(new_dict);
      g_free(new_lang);
    } else {
      swap_dictionary(new_dict, new_lang);
      ret = true;
    }
  }

  gtk_widget_destroy(dialog);

  return ret;
}

static bool select_lang () {
  bool ret = false;

  GtkWidget *dialog = gtk_dialog_new_with_buttons("Wybór języka", NULL, 0,
                                                  GTK_STOCK_OK,
                                                  GTK_RESPONSE_ACCEPT,
                                                  "Dodaj nowy",
                                                  CUSTOM_RESPONSE_ADD,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_REJECT,
                                                  NULL);

  GtkWidget *vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

  GtkWidget *label = gtk_label_new("Wybierz język lub dodaj nowy");
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 1);

  // Spuszczane menu
  char *lang_list;
  size_t list_len;

  if (dictionary_lang_list(&lang_list, &list_len) < 0) {
    error_dialog("Nie udało się wczytać listy języków");
    gtk_widget_destroy(dialog);
    return false;
  }

  GtkWidget *combo = gtk_combo_box_text_new();

  char *new_lang = 0;
  while ((new_lang = argz_next(lang_list, list_len, new_lang))) {
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), new_lang);
  }

  free(lang_list);

  gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
  gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 1);
  gtk_widget_show(combo);

  int response = gtk_dialog_run(GTK_DIALOG(dialog));
  if (response == GTK_RESPONSE_ACCEPT) {
    new_lang = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

    struct dictionary *new_dict = dictionary_load_lang(new_lang);

    if (!new_dict) {
      error_dialog("Nie udało się wczytać słownika dla wybranego języka.");
      g_free(new_lang);
    } else {
      swap_dictionary(new_dict, new_lang);
      ret = true;
    }
  } else if (response == CUSTOM_RESPONSE_ADD) {
    ret = add_lang();
  }

  if (ret) {
    clear_all_highlighted();
    check_buffer();
  }

  gtk_widget_destroy(dialog);

  return ret;
}

static bool add_word (wchar_t *word) {
  dictionary_insert(dict, word);

  if (dictionary_save_lang(dict, lang) < 0) {
    error_dialog("Nie udało się zapisać słowa");
    return false;
  }

  clear_all_highlighted();
  check_buffer();

  return true;
}

// Procedurka obsługi
static void check_word (GtkMenuItem *item, gpointer data) {
  GtkWidget *dialog;
  GtkTextIter start, end;
  gunichar *wword;

  // Znajdujemy pozycję kursora
  gtk_text_buffer_get_iter_at_mark(editor_buf, &end,
                                   gtk_text_buffer_get_insert(editor_buf));

  // Jeśli nie wewnątrz słowa, kończymy
  if (!is_on_word(&end)) {
    error_dialog("Kursor musi być na słowie");
    return;
  }

  if (dict == NULL) {
    if (!select_lang()) {
      error_dialog("Sprawdzanie pisowni nie jest możliwe – nie udało się wybrać języka");
      return;
    };
  }

  wword = get_word(&end);
  start = get_word_start_iter(&end);

  // Sprawdzamy
  if (dictionary_find(dict, (wchar_t *)wword)) {
    dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                    "Wszystko w porządku,\nśpij spokojnie");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
  }
  else {
    // Czas korekty
    GtkWidget *vbox, *label, *combo;
    struct word_list hints;
    int i;
    const wchar_t * const * words;

    dictionary_hints(dict, (wchar_t *)wword, &hints);
    words = word_list_get(&hints);

    // Tekst
    if (word_list_size(&hints) == 0) {
      dialog = gtk_dialog_new_with_buttons("Korekta", NULL, 0,
                                           GTK_STOCK_OK,
                                           CUSTOM_RESPONSE_ADD,
                                           GTK_STOCK_CANCEL,
                                           GTK_RESPONSE_REJECT,
                                           NULL);

      vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

      label = gtk_label_new("Nie znaleziono żadnych podpowiedzi, dodać słowo do słownika?");
      gtk_widget_show(label);
      gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 1);
    } else {
      dialog = gtk_dialog_new_with_buttons("Korekta", NULL, 0,
                                           GTK_STOCK_OK,
                                           GTK_RESPONSE_ACCEPT,
                                           "Dodaj do słownika",
                                           CUSTOM_RESPONSE_ADD,
                                           GTK_STOCK_CANCEL,
                                           GTK_RESPONSE_REJECT,
                                           NULL);

      vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

      label = gtk_label_new("Wybierz podpowiedź lub dodaj to słowo do słownika");
      gtk_widget_show(label);
      gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 1);

      // Spuszczane menu
      combo = gtk_combo_box_text_new();
      for (i = 0; i < word_list_size(&hints); i++) {
        // Combo box lubi mieć Gtk
        char *uword = g_ucs4_to_utf8((gunichar *)words[i], -1, NULL, NULL, NULL);

        // Dodajemy kolejny element
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), uword);
        g_free(uword);
      }
      gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
      gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 1);
      gtk_widget_show(combo);
    }

    word_list_done(&hints);

    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
      char *korekta =
        gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

      // Usuwamy stare
      gtk_text_buffer_delete(editor_buf, &start, &end);
      // Wstawiamy nowe
      gtk_text_buffer_insert(editor_buf, &start, korekta, -1);
      g_free(korekta);
    } else if (response == CUSTOM_RESPONSE_ADD) {
      add_word((wchar_t *)wword);
    }
    gtk_widget_destroy(dialog);
  }
  g_free(wword);
}

static void toggle_spellcheck(GtkCheckMenuItem *spellcheck_item) {
  if (gtk_check_menu_item_get_active(spellcheck_item)) {
    if (dict == NULL) {
      if (!select_lang()) {
        error_dialog("Sprawdzanie pisowni nie jest możliwe – nie udało się wybrać języka");
        gtk_check_menu_item_set_active(spellcheck_item, false);
        return;
      };
    }

    check_buffer();

    g_signal_connect(G_OBJECT(editor_buf), "changed",
                     G_CALLBACK(check_at_cursor), NULL);
  } else {
    clear_all_highlighted();
    g_signal_handlers_disconnect_by_func(G_OBJECT(editor_buf),
                                         G_CALLBACK(check_buffer), NULL);
  }
}

static void setup() {
  gtk_text_buffer_create_tag(editor_buf, "misspelled",
                             "foreground", "red", NULL);
}

// Tutaj dodacie nowe pozycje menu
void extend_menu (GtkWidget *menubar) {
  GtkWidget *spell_menu_item, *spell_menu, *check_item, *spellcheck_item,
            *lang_item;

  spell_menu_item = gtk_menu_item_new_with_label("Spell");
  spell_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(spell_menu_item), spell_menu);
  gtk_widget_show(spell_menu_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), spell_menu_item);

  check_item = gtk_menu_item_new_with_label("Check word");
  g_signal_connect(G_OBJECT(check_item), "activate",
                   G_CALLBACK(check_word), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), check_item);
  gtk_widget_show(check_item);

  spellcheck_item = gtk_check_menu_item_new_with_label("Check while typing");
  g_signal_connect(G_OBJECT(spellcheck_item), "toggled",
                   G_CALLBACK(toggle_spellcheck), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), spellcheck_item);
  gtk_widget_show(spellcheck_item);

  lang_item = gtk_menu_item_new_with_label("Select language");
  g_signal_connect(G_OBJECT(lang_item), "activate",
                   G_CALLBACK(select_lang), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), lang_item);
  gtk_widget_show(lang_item);

  g_signal_connect(G_OBJECT(editor_window), "show",
                   G_CALLBACK(setup), NULL);

  g_signal_connect(G_OBJECT(editor_window), "destroy",
                   G_CALLBACK(delete_dictionary), NULL);
}

/*EOF*/
