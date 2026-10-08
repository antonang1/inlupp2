#include "stock.h"
#include "merch.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>
#include "../data_structures/hash_table.h"
#include "../data_structures/hash_table_iterator.h"
#include <stdbool.h>

// RULE:
// negative outcome: a before b
// zero: equal
// positive: a after b
int comparison_shelf(elem_t a, elem_t b)
{
    shelf_t *shelf_a = a.p;
    shelf_t *shelf_b = b.p;

    if (shelf_a == NULL || shelf_b == NULL) 
    {
        return 0;
    }

    return(strcmp(shelf_a->shelf_name, shelf_b->shelf_name));
}

shelf_t *shelf_create(char *shelf, char *merch_name)
{
    shelf_t *new_shelf = calloc(1, sizeof(shelf_t));
    new_shelf->shelf_name = shelf;
    new_shelf->merch_name = merch_name;

    return new_shelf;
}

static void shelf_destroy(elem_t shelf_info) 
{
    shelf_t *shelf = shelf_info.p;

    free(shelf->shelf_name);
    free(shelf->merch_name);
    free(shelf);

    return;
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
    ioopm_hash_table_insert(store->stock, string_elem(shelf_name), ptr_elem(new_shelf));

    // also add pointer to shelf in merch->shelfs
    merch_t *merch = result_merch_exists.p;
    ioopm_list_append(merch->shelfs, ptr_elem(new_shelf));

    return true;
}


// This function does not have ownership over the memeory allocated
// ui.c does when calling this function!!! 
elem_t *list_shelves(merch_t *merch, size_t *size_out)
{
    *size_out = ioopm_hash_table_size(merch->shelfs);
    elem_t *shelves = calloc(*size_out, sizeof(elem_t));

    ioopm_hash_table_iterator_t *iter = ioopm_list_iterator_create(merch->shelfs); 
    size_t i = 0;

    while (!ioopm_list_iterator_at_end(iter))
    {
        shelves[i].i = ioopm_list_iterator_current(iter);
        i++;
        ioopm_list_iterator_advance(iter);
    }

    ioopm_list_iterator_destroy(iter);
    insertion_sort(shelves, *size_out, comparison_shelf);

    return shelves;
}