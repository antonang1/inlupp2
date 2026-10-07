#pragma once

#include "../data_structures/hash_table.h"

// store_create()
// store_add_cart(s)
// store_remove_cart(s, id)
// store_get_cart(s, id, &out)
// store_next_id(s)

// The warehouse itself. 

// db_create()
// db_destroy(db)
// db_add_merch(db, name, desc, price)
// db_remove_merch(db, name)
// db_edit_merch(db, old, new, desc, price)
// db_get_merch(db, name, &out)
// db_replenish(db, shelf, name, qty)
// db_stock_level(db, name)
// db_total_stock_in_carts(db, name)
typedef struct store store_t;

struct store
{
    ioopm_hash_table_t *merchandise;
    ioopm_hash_table_t *stock;
    ioopm_hash_table_t *carts;
    size_t next_cart_id;
};

