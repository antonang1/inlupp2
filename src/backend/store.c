#include "store.h"
#include "merch.h"
#include "stock.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "../data_structures/hash_table.h"
#include "../utils/utils.h"

ioopm_hash_table_t *store_stock(store_t *store)
{
    return store->stock;
}

ioopm_hash_table_t *store_merchandise(store_t *store)
{
    return store->merchandise;
}


static void merch_destroy_callback(elem_t value)
{
    merch_t *merch = value.p;
    
    if (merch == NULL)
    {
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

static void shelf_destroy_callback(elem_t shelf_info) 
{

  shelf_t *shelf = shelf_info.p;


  free(shelf->shelf_name);
  free(shelf->merch_name);
  free(shelf);

  return;
}

void store_destroy(store_t *store) {
  if (store == NULL)
  {
    return;
  }

  ioopm_hash_table_destroy(store->stock);
  ioopm_hash_table_destroy(store->merchandise);

  free(store);  

}

store_t *store_create(void) {
  store_t *new_store = calloc(1, sizeof(store_t));

  // TODO: lägg till resten av ht's också.
  new_store->merchandise = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare, merch_destroy_callback);
  new_store->stock = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare, shelf_destroy_callback);

  return new_store;
}