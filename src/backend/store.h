#pragma once

#include "../data_structures/hash_table.h"

typedef struct store store_t;

struct store
{
    ioopm_hash_table_t *merchandise;
    ioopm_hash_table_t *stock;
    ioopm_hash_table_t *carts;
    size_t next_cart_id;
};

/// @brief Returns the store's stock table.
/// @param store The store to read from.
/// @return The hash table holding every shelf. Owned by the store.
ioopm_hash_table_t *store_stock(store_t *store);

/// @brief Returns the store's merchandise table.
/// @param store The store to read from.
/// @return The hash table holding every item. Owned by the store.
ioopm_hash_table_t *store_merchandise(store_t *store);

void store_destroy(store_t *store);

store_t *store_create(void);

int store_quit(store_t *store);
