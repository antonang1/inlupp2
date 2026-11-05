#pragma once

#include "../data_structures/hash_table.h"

/* 
NOTE: Documentation outline generated with KLAR, then rewritten and verified by Signe von Greiff and Anton Äng.
No code in this repository was generated.
*/

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

/// @brief Frees the store and everything it owns.
/// @param store The store to destroy. Does nothing if store is NULL.
/// @note Destroys all three hash tables, which run their own destroy
///       callbacks on every entry. Every item, shelf and cart is freed
///       through those callbacks, so no separate free is needed.
/// @note Any pointer the caller still holds into the store becomes
///       invalid after this call.
void store_destroy(store_t *store);

/// @brief Creates an empty store with its three hash tables.
/// @return A new store, or NULL if allocation fails.
/// @note Creates merchandise, stock and carts, each with its own key
///       function, comparison function and destroy callback.
/// @note The caller owns the returned store and must free it with
///       store_destroy.
store_t *store_create(void);

/// @brief Destroys the store and signals that the program should exit.
/// @param store The store to destroy.
/// @return Always 1, which the caller reads as "quit".
/// @note Calls store_destroy, so the same ownership rules apply.
int store_quit(store_t *store);
