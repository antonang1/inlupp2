#include "stock.h"
#include "merch.h"
#include "store.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>
#include "../data_structures/hash_table.h"
#include "../data_structures/hash_table_iterator.h"
#include "../data_structures/list_iterator.h"
#include "../data_structures/linked_list.h"
#include <stdbool.h>
#include <stdio.h>


// RULE:
// negative outcome: a before b
// zero: equal
// positive: a after b
int comparison_shelf(elem_t a, elem_t b)
{
    return strcmp(a.s, b.s);
    /*
    shelf_t *shelf_a = a.p;
    shelf_t *shelf_b = b.p;

    if (shelf_a == NULL || shelf_b == NULL) 
    {
        return 0;
    }
    return(strcmp(shelf_a->shelf_name, shelf_b->shelf_name));
    */

}

// Allocates the struct and a copy of both strings (strdup).
// The strings are copied, so the caller keeps nothing.
// Free everything with shelf_destroy.
shelf_t *shelf_create(const char *shelf, const char *merch_name)
{
    shelf_t *new_shelf = calloc(1, sizeof(shelf_t));
    new_shelf->shelf_name = strdup(shelf);
    new_shelf->merch_name = strdup(merch_name);

    return new_shelf;
}

// ändra på shelf_t *shelf till att plocka in faktiska värdena (gör samma för merch_add_to_store)
bool replenish_stock(store_t *store, char *shelf_name, char *merch_name, size_t quantity)
{
    if (quantity < (size_t) 1)
    {
        return false;
    }
    elem_t result_merch_exists;
    // check if merch exists in store->merchandise
    if (!ioopm_hash_table_lookup(store->merchandise, string_elem(merch_name), &result_merch_exists))
    {
        return false;
    }
    
    elem_t result_shelf_exists;
    // check if shelf exists in store->stock
    if (ioopm_hash_table_lookup(store->stock, string_elem(shelf_name), &result_shelf_exists))
    {
        shelf_t *shelf = result_shelf_exists.p;
        
        // if given shelf exists but its merch does not match with input merch_name, non-valid inputs were given.
        if (strcmp(shelf->merch_name, merch_name) != 0)
        {
            return false;
        }
        
        // increment stock at shelf, effectivly also increasing the quantity in merch->shelfs becuase of aliasing.
        shelf->quantity += quantity;
        
        
        return true;
    }
    
    // the storage location does not exist in store->stock, therefore add shelf:
    shelf_t *new_shelf = shelf_create(shelf_name, merch_name);
    new_shelf->quantity = quantity;
    
    ioopm_hash_table_insert(store->stock, string_elem(new_shelf->shelf_name), ptr_elem(new_shelf));
    
    // also add pointer to shelf in merch->shelfs
    merch_t *merch = result_merch_exists.p;

    // if merch does not contain any locations, initilize shelfs to a list.
    if (merch->shelfs == NULL)
    {
        merch->shelfs = ioopm_list_create(NULL);
    }
    ioopm_list_append(merch->shelfs, ptr_elem(new_shelf));

    return true;
}

bool stock_remove_merch(store_t *store, merch_t *merch_to_clear)
{
    ioopm_list_t *shelfs_of_merch = merch_to_clear->shelfs;
    ioopm_list_node_t *current = shelfs_of_merch->first;
    
    elem_t removed;

    while (current != NULL)
    {
        shelf_t *current_shelf = current->head.p;

        bool remove_success = ioopm_hash_table_remove(store->stock, string_elem(current_shelf->shelf_name), &removed);
        
        if (!remove_success)
        {
            return false;
        }

        current = current->tail;
    }

    return true;
}

// ska denna kanske vara i merch.c?
void stock_update_merch_name(merch_t *edited_merch)
{
    char *name_of_merch = edited_merch->name;
    ioopm_list_t *shelfs_of_merch = edited_merch->shelfs;
    ioopm_list_node_t *current = shelfs_of_merch->first;

    // traverse list and change merch_name of each shelf_t
    while (current != NULL)
    {
        shelf_t *current_shelf = current->head.p;

        free(current_shelf->merch_name);
        current_shelf->merch_name = strdup(name_of_merch);
        
        current = current->tail;
    }
}

// This function does not have ownership over the memeory allocated
// ui.c does when calling this function!!! 
elem_t *list_shelves(merch_t *merch, size_t *size_out)
{
    if (merch->shelfs == NULL)
    {
        *size_out = 0;
        return NULL;
    }

    *size_out = ioopm_list_size(merch_shelves(merch));
    elem_t *shelves = calloc(*size_out, sizeof(elem_t));
    
    ioopm_list_iterator_t *iter = ioopm_list_iterator_create(merch_shelves(merch));
    size_t i = 0;
    while (!ioopm_list_iterator_at_end(iter))
    {
        shelf_t *current_shelf = ioopm_list_iterator_current(iter).p;
        shelves[i] = string_elem(current_shelf->shelf_name);
        i++;
        ioopm_list_iterator_advance(iter);
    }
    

    ioopm_list_iterator_destroy(iter);
    insertion_sort(shelves, *size_out, comparison_shelf);

    return shelves;
}