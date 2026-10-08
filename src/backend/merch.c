#include "merch.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>
#include "../data_structures/hash_table.h"
#include "../data_structures/hash_table_iterator.h"


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


static void merch_destroy(merch_t *merch)
{
    free(merch->name);
    free(merch->desc);
    free(merch);
    
    return;
}

merch_t *merch_create(char *name, char *desc, size_t price)
{
    merch_t *new_merch = calloc(1, sizeof(merch_t));

    new_merch->name = name;
    new_merch->desc = desc;
    new_merch->price = price;

    return new_merch;
}

// 2.1.1 Add Merchandise
bool merch_add_to_store(store_t *store, merch_t *merch)
{  
    elem_t tmp;
    if (ioopm_hash_table_lookup(store->merchandise, string_elem(merch->name), &tmp))
    {
        merch_destroy(merch);
        return false;
    }
    ioopm_hash_table_insert(store->merchandise, string_elem(merch->name), ptr_elem(merch));
    
    return true;

}

// 2.1.3 Remove Merchandise
bool merch_remove_from_store(store_t *store, merch_t *merch)
{
    elem_t tmp;
    if (!ioopm_hash_table_lookup(store->merchandise, string_elem(merch->name), &tmp))
    {
        return false;
    }

    ioopm_list_destroy(merch->shelfs);
    merch_destroy(merch);

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

    merch_of_intrest->name = new_name;
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