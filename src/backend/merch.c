#include "merch.h"
#include "store.h"
#include "stock.h"

ioopm_list_t *ioopm_merch_shelves(merch_t *merch)
{
    return merch->shelfs;
}

int ioopm_merch_price(merch_t *merch)
{
    return merch->price;
}

// RULE:
// negative outcome: a before b
// zero: equal
// positive: a after b
int ioopm_comparison_merch(elem_t a, elem_t b)
{
    return strcmp(a.s, b.s);
}

void ioopm_merch_destroy(merch_t *merch)
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

merch_t *ioopm_merch_create(const char *name, const char *desc, size_t price)
{
    merch_t *new_merch = calloc(1, sizeof(merch_t));

    new_merch->name = strdup(name);
    new_merch->desc = strdup(desc);
    new_merch->price = price;

    return new_merch;
}

// 2.1.1 Add Merchandise
bool ioopm_merch_add_to_store(store_t *store, char *name, char *desc, size_t price)
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

    merch_t *new_merch = ioopm_merch_create(name, desc, price);
    ioopm_hash_table_insert(store->merchandise, string_elem(new_merch->name), ptr_elem(new_merch));
    
    return true;

}

// 2.1.3 Remove Merchandise
bool ioopm_merch_remove_from_store(store_t *store, char *merch_name)
{
    elem_t merch_of_intrest;
    if (!ioopm_hash_table_lookup(store->merchandise, string_elem(merch_name), &merch_of_intrest))
    {
        return false;
    }

    merch_t *merch = merch_of_intrest.p;

    if (merch->shelfs != NULL)
    {
        ioopm_stock_remove_merch(store, merch); // merch apperance in stock needs to be removed aswell
    }

    ioopm_hash_table_remove(store->merchandise, string_elem(merch_name), &merch_of_intrest);    // then we destroy the merch

    return true;
}


// 2.1.4 Edit Merchandise
bool ioopm_merch_edit(store_t *store, char *old_name, char *new_name, char *new_desc, size_t new_price)
{
    if (new_name == NULL)
    {
        return false;
    }

    // check if old_name is a valid key to an entry (merch)
    elem_t result;
    if (!ioopm_hash_table_lookup(store->merchandise, string_elem(old_name), &result))
    {
        return false;
    }

    // check if old and new name are not equal, and if new name already exists;
    if (strcmp(old_name, new_name) != 0 &&
        ioopm_hash_table_has_key(store->merchandise, string_elem(new_name)))
    {
        return false;
    }

    merch_t *merch_of_intrest = result.p;
    
    // check if old and new name are equal, then we do a simpler procedure where only desc and price is updated.
    // no need to update name, therefore no need to rehash the entry (merch)
    if (strcmp(old_name, new_name) == 0)
    {
        free(merch_of_intrest->desc);
        merch_of_intrest->desc = strdup(new_desc);
        merch_of_intrest->price = new_price;
        return true;
    }

    elem_t extracted_merch;
    if (!ioopm_hash_table_extract(store->merchandise, string_elem(old_name), &extracted_merch))
    {
        return false;
    }

    merch_t *merch_update = extracted_merch.p;
    free(merch_of_intrest->name);
    free(merch_of_intrest->desc);
    
    merch_update->name = strdup(new_name);
    merch_update->desc = strdup(new_desc);
    merch_update->price = new_price;
    
    ioopm_hash_table_insert(store->merchandise, string_elem(merch_update->name), ptr_elem(merch_update));
    
    // if merch has locations, their info needs to be updated to new name aswell.
    if (merch_update->shelfs != NULL)
    {
        ioopm_stock_update_merch_name(merch_update);
    }
    
    return true;
}

// This function does not have ownership over the memeory allocated
// ui.c does when calling this function!!! 
elem_t *ioopm_list_merchandise(store_t *store, size_t *size_out)
{
    *size_out = ioopm_hash_table_size(store->merchandise);
    elem_t *merch = calloc(*size_out, sizeof(elem_t));

    ioopm_hash_table_iterator_t *iter = ioopm_hash_table_iterator_create(store->merchandise);
    size_t i = 0;

    while (!ioopm_hash_table_iterator_at_end(iter))
    {
        merch_t *current_merch = ioopm_hash_table_iterator_current_value(iter).p;
        merch[i] = string_elem(current_merch->name);
        i++;
        ioopm_hash_table_iterator_advance(iter);
    }

    ioopm_hash_table_iterator_destroy(iter);
    insertion_sort(merch, *size_out, ioopm_comparison_merch);

    return merch;
}