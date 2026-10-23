#pragma once
#include "../utils/utils.h"
#include "../backend/store.h"
// One function per action code, each reading its own input lines and calling exactly one backend function.

// ui_add_merch(db) : A
// ui_list_merch(db) : L
// ui_remove_merch(db) : D
// ui_edit_merch(db) : E
// ui_show_stock(db, store) : S
// ui_replenish(db) : P
// ui_create_cart(db, store) : C
// ui_remove_cart(store) : R
// ui_add_to_cart(db, store) : +
// ui_remove_from_cart(store) : -
// ui_calc_cost(db, store) : =
// ui_checkout(db, store) : O
// ui_undo(db) : U
// ui_quit() : Q

bool is_valid_menu_choice(char *str);

answer_t convert_to_upper(char *str);

char ask_question_menu(char *question);

bool confirmation_starts_with(char *str, char expected);

bool confirmation_procedure(char *question, char expected, char *str_to_print_if_confrimed);

char *print_menu(void);

void print_merch(char *string);

void ui_add_merch(store_t *store);

void ui_list_merchandise(store_t *store);

void ui_remove_merchandise(store_t *store);

void ui_edit_merch(store_t *store);


int event_loop(store_t *store);