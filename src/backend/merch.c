#include "merch.h"
#include "stock.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>
#include "../data_structures/hash_table.h"
#include "../data_structures/hash_table_iterator.h"
#include "../data_structures/hash_table.h"
#include "../data_structures/linked_list.h"

ioopm_list_t *merch_shelves(merch_t *merch)
{
    return merch->shelfs;
}

int merch_price(merch_t *merch)
{
    return merch->price;
}

// RULE:
// negative outcome: a before b
// zero: equal
// positive: a after b
int comparison_merch(elem_t a, elem_t b)
{
    merch_t *merch_a = a.p;
    merch_t *merch_b = b.p;

    if (merch_a == NULL || merch_b == NULL) 
    {
        return 0;
    }

    return strcmp(merch_a->name, merch_b->name);
}

void merch_destroy(merch_t *merch)
{    
    if (merch == NULL)
    {
        free(merch);
        return;
    }

    if (merch->shelfs != NULL)
    {
        ioopm_list_destroy(merch->shelfs);
    }

    free(merch->name);
    free(merch->desc);
    free(merch);
    
    return;
}

merch_t *merch_create(const char *name, const char *desc, size_t price)
{
    merch_t *new_merch = calloc(1, sizeof(merch_t));

    new_merch->name = strdup(name);
    new_merch->desc = strdup(desc);
    new_merch->price = price;

    return new_merch;
}

//borde vi ha const på alla sånna här funcs?
// 2.1.1 Add Merchandise
bool merch_add_to_store(store_t *store, char *name, char *desc, size_t price)
{  
    if (store == NULL)
    {
        return false;
    }

    elem_t tmp;
    if (ioopm_hash_table_lookup(store->merchandise, string_elem(name), &tmp))
    {
        return false;
    }

    merch_t *new_merch = merch_create(name, desc, price);
    ioopm_hash_table_insert(store->merchandise, string_elem(new_merch->name), ptr_elem(new_merch));
    
    return true;

}

// 2.1.3 Remove Merchandise
bool merch_remove_from_store(store_t *store, char *merch_name)
{
    elem_t merch_of_intrest;
    if (!ioopm_hash_table_lookup(store->merchandise, string_elem(merch_name), &merch_of_intrest))
    {
        return false;
    }

    merch_t *merch = merch_of_intrest.p;

    if (merch->shelfs != NULL)
    {
        stock_remove_merch(store, merch); // merch apperance in stock needs to be removed aswell
    }

    ioopm_hash_table_remove(store->merchandise, string_elem(merch_name), &merch_of_intrest);    // then we destroy the merch

    return true;
}


// 2.1.4 Edit Merchandise
// TILL SENARE: KANSKE ÄNDRA SÅ ATT VI HAR IDS FÖR MAMN -> MERCH MAPPING I HT.
bool merch_edit(store_t *store, char *old_name, char *new_name, char *new_desc, size_t new_price)
{
    elem_t result;
    if (!ioopm_hash_table_lookup(store->merchandise, string_elem(old_name), &result))
    {
        return false;
    }
    if (strcmp(old_name, new_name) != 0 &&
        ioopm_hash_table_has_key(store->merchandise, string_elem(new_name)))
    {
        return false;
    }

    merch_t *merch_of_intrest = result.p;
    // Otherwise we have to rehash!
    char *tmp = merch_of_intrest->name;

    merch_of_intrest->name = new_name; //blir detta problem för att vi har strdup förut? måste vi frigöra name först?
    merch_of_intrest->desc = new_desc;
    merch_of_intrest->price = new_price;

    elem_t removed;
    ioopm_hash_table_remove(store->merchandise, string_elem(tmp), &removed);
    ioopm_hash_table_insert(store->merchandise, string_elem(merch_of_intrest->name), ptr_elem(merch_of_intrest));

    return true;
    
}


// This function does not have ownership over the memeory allocated
// ui.c does when calling this function!!! 
elem_t *list_merchandise(store_t *store, size_t *size_out)
{
    *size_out = ioopm_hash_table_size(store->merchandise);
    elem_t *merch = calloc(*size_out, sizeof(elem_t));

    ioopm_hash_table_iterator_t *iter = ioopm_hash_table_iterator_create(store->merchandise);
    size_t i = 0;

    while (!ioopm_hash_table_iterator_at_end(iter))
    {
        merch[i] = ioopm_hash_table_iterator_current_value(iter);
        i++;
        ioopm_hash_table_iterator_advance(iter);
    }

    ioopm_hash_table_iterator_destroy(iter);
    insertion_sort(merch, *size_out, comparison_merch);

    return merch;
}