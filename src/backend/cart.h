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

bool add_to_cart(store_t *store, size_t id, char *name, size_t quantity);

bool remove_cart(store_t *store, size_t id);

bool remove_from_cart(store_t *store, size_t id, char *name, size_t quantity);