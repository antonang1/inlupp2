
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

static bool is_valid_menu_choice(char *str) {
    int str_len = strlen(str);

    //check if valid str
    if (str_len != 1) {
        return false;
    }
    
    bool valid = strchr("AaLlDdEeSsPpCcRr+-=OoUuQq", str[0]) != NULL; // checks if str[0] is the same as something in left argument.

    return valid;
}

static answer_t convert_to_upper(char *str) {
    answer_t answer;
    answer.int_value = toupper(str[0]);

    return answer;
}

static char ask_question_menu(char *question) {
    return ask_question(question, is_valid_menu_choice, convert_to_upper).int_value;
}

static bool confirmation_starts_with(char *str, char expected)
{
    if (str == NULL || str[0] == '\0')
    {
        return false;
    }

    return toupper((unsigned char) str[0]) == expected;
}

static bool confirmation_procedure(char *question, char expected, char *str_to_print_if_confrimed)
{
    char *confirmation = ask_question_string(question);

    if (confirmation_starts_with(confirmation, expected))
    {
        printf("%s", str_to_print_if_confrimed);

        free(confirmation);
        
        return true;
    }
    
    free(confirmation);
    return false;
}

static char *print_menu(void) {
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

static void print_merch(char *string)
{
    printf("Name: %s\n", string);
}

static void print_shelf(shelf_t *shelf)
{
    printf("Shelf: %s\n ->Quantity: %zu\n", shelf->shelf_name, shelf->quantity);

    return;
}

static void ui_add_merch(store_t *store)
{
    printf("-----ADD MERCH TO STORE-----\n");
    char *merch_name = ask_question_string("Name of new merch: ");
    char *merch_desc = ask_question_string("Description of merch: ");
    size_t merch_price = (size_t) ask_question_int("Price of merch: ", is_positive_number);

    merch_add_to_store(store, merch_name, merch_desc, merch_price);
    
    free(merch_name);
    free(merch_desc);

}

static void ui_list_merchandise(store_t *store)
{
    printf("-----LIST MERCHANDISE-----\n");
    size_t size = 0;
    elem_t *array_of_merch = list_merchandise(store, &size);

    for (size_t i = 0; i < size; i++)
    {
        if (i != 0 && i % 2 == 0)
        {
            if (confirmation_procedure("Do you want to keep printing merchandise? [Y/N]: ", 'N', "Returning to menu!\n"))
            {
                break;
            }
        }

        print_merch(array_of_merch[i].s);
    }
    
    free(array_of_merch);

    return;
}

static void ui_remove_merchandise(store_t *store)
{   
    printf("-----REMOVE MERCH-----\n");

    bool done = false;

    do {
        char *merch_of_intrest = ask_question_string("Name of merch to remove from store: ");
        
        if (!confirmation_procedure("Are you sure that you want to remove this merch from store? [Y/N]: ", 'Y', "REMOVING MERCH FROM STORE\n"))
        {
            printf("Removal not executed\nReturning to menu!\n");
            return;
        }
    
        bool remove_success = merch_remove_from_store(store, merch_of_intrest);
        if (remove_success)
        {
            printf("The following merch has been removed from store: '%s'\n", merch_of_intrest);
            free(merch_of_intrest);
            done = true;
        }
        else
        {
            printf("FAILED TO REMOVE: merch not found\n");

            if (confirmation_procedure("Do you wish to try again? [Y/N]: ", 'N', "Returning to menu!\n"))
            {
                done = true;
            }
        } 

        free(merch_of_intrest);
    } while (done == false);

    return;
}

static void ui_edit_merch(store_t *store)
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
            printf("Edit not completed\nReturning to menu!\n");

            free(merch_to_edit);
            free(new_name);
            free(new_desc);
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
            
            if (confirmation_procedure("Do you wish to try again? [Y/N]: ", 'N', "Returning to menu!\n"))
            {
                done = true;
            }
        }
        
    free(merch_to_edit);
    free(new_name);
    free(new_desc);

    } while (done == false);

    return;
}

static void ui_show_stock(store_t *store)
{
    printf("-----SHOW STOCK-----\n");
    char *merch_name = ask_question_string("Name of merch: ");


    elem_t merch_of_intrest;
    if (!ioopm_hash_table_lookup(store->merchandise, string_elem(merch_name), &merch_of_intrest))
    {
        printf("FAILED TO SHOW STOCK: name of merch is does not exist in store\nReturning to menu!\n");
        
        free(merch_name);
        return;
    }

    merch_t *merch = merch_of_intrest.p;
    
    size_t size = 0;
    elem_t *array_of_shelfs = list_shelves(merch, &size);

    if (array_of_shelfs == NULL)
    {
        printf("Merch does not have any stock.\nReturning to menu!\n");

        free(array_of_shelfs);
        free(merch_name);
        return;
    }

    for (size_t i = 0; i < size; i++)
    {
        elem_t result;
        ioopm_hash_table_lookup(store->stock, array_of_shelfs[i], &result);
        shelf_t *shelf = result.p;
        print_shelf(shelf);
    }

    free(array_of_shelfs);
    free(merch_name);
    
    return;
}

static void ui_replenish(store_t *store)
{
    printf("-----REPLENISH STOCK-----\n");

    char *shelf = ask_question_shelf("Enter a shelf ID (e.g. A10): ");
    char *name = ask_question_string("Enter merchandise name to replenish: ");
    size_t quantity = (size_t) ask_question_int("Quantity of merchandise: ", is_positive_number);

    bool replenished = replenish_stock(store, shelf, name, quantity);

    if (!replenished)
    {
        printf("Could not replenish stock. Please check that:\n"
               "  -> The shelf is empty or already contains this merchandise.\n"
               "  -> The merchandise exists in the store.\n"
               "  -> The quantity is at least 1.\n");
        
        free(shelf);
        free(name);
        return;
    }
    
    elem_t result;
    ioopm_hash_table_lookup(store->stock, string_elem(shelf), &result);
    shelf_t *shelf_data = result.p;

    printf("Replenish completed. \n");
    print_shelf(shelf_data);

    free(shelf);
    free(name);
    return;
}

static void ui_create_cart(store_t *store)
{
    cart_t *new_cart = cart_create(store->next_cart_id);
    add_cart_to_store(store, new_cart);

    printf("A new cart has been created!\nCart id: '%zu'\nReturning to menu!", new_cart->id);

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
        else if (user_choice == 'S')
        {
            ui_show_stock(store);            
        } 
        else if (user_choice == 'P') 
        {
            ui_replenish(store);
        } 
        else if (user_choice == 'C') 
        {
          ui_create_cart(store);  
          // vill vi skapa en funktion som listar alla carts som finns och en functino som listar alla items i en viss cart?
        } 
        /*
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