#pragma once
#include "data_structures/db.c"
#include "utils.h"
#include "backend/merch.h"
#include "backend/cart.h"
#include "backend/stock.h"
#include "../backend/store.h"


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
// ui_quit() : Q

// MENU
// Needed because toupper() takes an int as arg but a convert_func expects a char * as arg

bool is_valid_menu_choice(char *str) {
    int str_len = strlen(str);

    //check if valid str
    if (str_len != 1) {
        return false;
    }
    
    bool valid = strchr("AaLlDdEeSsPpCcRr+-=OoUuQq", str[0]) != NULL; // checks if str[0] is the same as something in left argument.

    return valid;
}

answer_t convert_menu(char *str) {
    answer_t answer;
    answer.int_value = toupper(str[0]);

    return answer;
}

char ask_question_menu(char *question) {
    return ask_question(question, is_valid_menu_choice, convert_menu).int_value;
}

bool check_confirmation(char *str)
{
    return (strcmp(str[0], "y") == 0 || strcmp(str[0], "Y") == 0);
}

char *print_menu(void) {
    // Added option [d], felt like it was missing
    char *menu = ("|--- MENU ---|"
                  "-> [A]dd merchandise\n"
                  "-> [L]ist merchandise\n"
                  "-> [D]elete merchandise\n"
                  "-> [E]dit merchandise\n"
                  "-> [S]how stock\n"
                  "-> Re[p]lenish\n"
                  "-> [C]reate cart\n"
                  "-> [R]emove cart\n"
                  "-> Add to cart: [+]\n"
                  "-> Remove from cart: [-]\n"
                  "-> Calculate cost: [=]\n"
                  "-> Check[o]ut\n"
                  "-> [Q]uit\n");
    
    return menu;
}


void ui_add_merch(store_t *store)
{
    char *merch_name = ask_question_string("Name of new merch: ");
    char *merch_desc = ask_question_string("Description of merch: ");
    size_t *merch_price = abs(ask_question_int("Price of merch: "));

    merch_add_to_store(store, merch_name, merch_desc, merch_price);
}

void print_merch(char *string)
{
    printf("Name: %s\n", string);
}

void ui_list_merchandise(store_t *store)
{
    size_t size = 0;
    elem_t *array_of_merch = list_merchandise(store, &size);

    for (size_t i = 0; i <= size; i++)
    {
        if (i % 20 == 0)
        {
            char *keep_printing = ask_question_string("Do you want to keep printing merchandise? [Y/N]");
            if (!check_confirmation(keep_printing))
            {
                break;
            }
        }
        print_merch(array_of_merch[i].s);
    }
    
    return;
}

void ui_remove_merchandise(store_t *store)
{
    char *merch_of_intrest = ask_question_string("Name of merch to remove from store: ");
    
    char *confirmation = ask_question_string("Are you sure that you want to remove this merch from store? [Y/N]");
    if (!check_confirmation(confirmation))
    {
        printf("Removal not executed\n");
        return;
    }

    merch_remove_from_store(store, merch_of_intrest);
    printf("The following merch has been removed from store: %s\n", merch_of_intrest);
    return; 
}

int event_loop(store_t *store) {
    bool run = true;
    char user_choice;

    while (run) {
        user_choice = ask_question_menu(print_menu());
        if (user_choice == 'A')
        {
            ui_add_merch(store);
        } 
        else if (user_choice == 'L')   
        {
             ui_list_merchandise(store);
        }
        else if (user_choice == 'D')
        {
            ui_remove_merchandise(store);   
        }
        else if (user_choice == 'E')
        {
            
        } 
        else if (user_choice == 'S')
        {
            
        } 
        else if (user_choice == 'P') 
        {
            
        } 
        else if (user_choice == 'C') 
        {
            
        } 
        else if (user_choice == 'R') 
        {
            
        } 
        else if (user_choice == '+') 
        {
            
        } 
        else if (user_choice == '-') 
        {
            
        } 
        else if (user_choice == '=') 
        {
            
        } 
        else if (user_choice == 'O') 
        {
            
        } 
        else if (user_choice == 'Q') 
        {
            run = false;
        }
        
    }
    return 1;
}