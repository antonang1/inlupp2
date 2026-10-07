#include "merch.h"

merch_t *merch_create(char *name, char *desc, size_t price)
{
    merch_t *new_merch = calloc(1, sizeof(merch_t));

    new_merch->name = name;
    new_merch->desc = desc;
    new_merch->price = price;

    return new_merch;
}

bool merch_add_to_store(store_t *store, merch_t *merch)
{  
    elem_t *tmp;
    if (ioopm_hash_table_lookup(store, string_elem(merch->name), tmp))
    {
        
    }
    ioopm_hash_table_insert(store->merchandise, string_elem(merch->name), ptr_elem(merch));


}

bool merch_remove_from_store(store_t *store, merch_t *merch)
{

}

bool merch_edit(store_t *store, char *old_name, char *new_name, char *new_desc, size_t new_price)
{

}