#pragma once

#include "../data_structures/hash_table.h"
#include "../data_structures/linked_list.h"
#include "store.h"
#include "merch.h" 

// cart_create(id)
// cart_destroy(c)
// cart_add(c, db, name, qty)
// cart_remove(c, name, qty) 
// cart_cost(c, db)
// cart_is_empty(c) 

typedef struct cart cart_t;

struct cart
{
    size_t id;
    ioopm_hash_table_t *items;
};

cart_t *cart_create(size_t id);

void add_cart_to_store(store_t *store, cart_t *cart); //ANTON LAGGT TILL DENNA, förut var man tvungen att inserta själv o de kändes inte som att fronend skulle göra det, förut var också next_valid_id eller vad den heter i store alltid samma, ändradet alldrig.

bool add_to_cart(store_t *store, size_t id, char *name, size_t quantity);

bool remove_cart(store_t *store, size_t id);

bool remove_from_cart(store_t *store, size_t id, char *name, size_t quantity);

int calculate_cost(store_t *store, size_t id);

void stock_decrease_merch(store_t *store, ioopm_hash_table_t *items_of_cart); 

bool check_if_all_items_valid(store_t *store, size_t id, ioopm_hash_table_t *items_of_cart, int *status);

int checkout(store_t *store, size_t id); 