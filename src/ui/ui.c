
#include "ui.h"
#include "../utils/utils.h"
#include "../backend/merch.h"
#include "../backend/cart.h"
#include "../backend/stock.h"
#include "../backend/store.h"
#include <stdio.h>
#include <ctype.h>


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

answer_t convert_to_upper(char *str) {
    answer_t answer;
    answer.int_value = toupper(str[0]);

    return answer;
}

char ask_question_menu(char *question) {
    return ask_question(question, is_valid_menu_choice, convert_to_upper).int_value;
}

bool confirmation_starts_with(char *str, char expected)
{
    if (str == NULL || str[0] == '\0')
    {
        return false;
    }

    return toupper((unsigned char) str[0]) == expected;
}

bool confirmation_procedure(char *question, char expected, char *str_to_print_if_confrimed)
{
    char *confirmation = ask_question_string(question);

    if (confirmation_starts_with(confirmation, expected))
    {
        printf("%s", str_to_print_if_confrimed);
        return true;
    }
    
    return false;
}

char *print_menu(void) {
    // Added option [d], felt like it was missing
    char *menu = ("\n\n   |----- MENU -----|\n"
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
                  "-> [Q]uit\n\n");
    
    return menu;
}

void print_merch(char *string)
{
    printf("Name: %s\n", string);
}

void ui_add_merch(store_t *store)
{
    printf("-----ADD MERCH TO STORE-----\n");
    char *merch_name = ask_question_string("Name of new merch: ");
    char *merch_desc = ask_question_string("Description of merch: ");
    size_t merch_price = (size_t) ask_question_int("Price of merch: ", is_positive_number);

    merch_add_to_store(store, merch_name, merch_desc, merch_price);

}

void ui_list_merchandise(store_t *store)
{
    printf("-----LIST MERCHANDISE-----\n");
    size_t size = 0;
    elem_t *array_of_merch = list_merchandise(store, &size);

    for (size_t i = 0; i < size; i++)
    {
        if (i != 0 && i % 2 == 0)
        {
            if (confirmation_procedure("Do you want to keep printing merchandise? [Y/N]: ", 'N', "RETURNING TO MENU\n"))
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
    printf("-----REMOVE MERCH-----\n");

    bool done = false;

    do {
        char *merch_of_intrest = ask_question_string("Name of merch to remove from store: ");
        
        if (!confirmation_procedure("Are you sure that you want to remove this merch from store? [Y/N]: ", 'Y', "REMOVING MERCH FROM STORE\n"))
        {
            printf("Removal not executed\nRETURNING TO STORE\n");
            return;
        }
    
        bool remove_success = merch_remove_from_store(store, merch_of_intrest);
        if (remove_success)
        {
            printf("The following merch has been removed from store: '%s'\n", merch_of_intrest);
            done = true;
        }
        else
        {
            printf("FAILED TO REMOVE: merch not found\n");

            if (confirmation_procedure("Do you wish to try again? [Y/N]: ", 'N', "RETURNING TO MENU\n"))
            {
                done = true;
            }
        } 

    } while (done == false);

    return;
}

void ui_edit_merch(store_t *store)
{
    printf("-----EDIT MERCH-----\n");

    bool done = false;

    do
    {
        char *merch_to_edit = ask_question_string("Name of merch to edit: ");
        
        char *new_name = ask_question_string("New name: ");
        char *new_desc = ask_question_string("New description: ");
        int new_price = ask_question_int("New price (positive integer): ", is_positive_number);
        
        if (!confirmation_procedure("Do you wish to complete the edit? [Y/N]: ", 'Y', "COMPLETING EDIT\n"))
        {
            printf("Edit not completed\nRETURNING TO MENU\n");
            return;
        }

        bool edit_success = merch_edit(store, merch_to_edit, new_name, new_desc, (size_t) new_price);
        if (edit_success)
        {
            printf("Edit completed: '%s' is now '%s'", merch_to_edit, new_name);
            done = true;
        }
        else
        {
            printf("FAILED TO EDIT: Name of merch to edit was invalid or new name is already taken.\n");
            
            if (confirmation_procedure("Do you wish to try again? [Y/N]: ", 'N', "RETURNING TO MENU\n"))
            {
                done = true;
            }
        }

    } while (done == false);

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
            ui_edit_merch(store);   
        } 
        /*
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
        */
        else if (user_choice == 'Q') 
        {
            run = false;
        }
        
    }
    return 1;
}