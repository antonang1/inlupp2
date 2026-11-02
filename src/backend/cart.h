#pragma once

#include "../data_structures/hash_table.h"
#include "../data_structures/linked_list.h"
#include "store.h"
#include "merch.h" 

typedef struct cart cart_t;

struct cart
{
    size_t id;
    ioopm_hash_table_t *items;
};

/// @brief Creates an empty cart with the given id.
/// @param id The cart's id, which becomes its key in store->carts.
/// @return A new cart, or NULL if allocation fails.
/// @note Allocates the struct and the items table. The caller owns the
///       cart until add_cart_to_store hands it over to the store.
cart_t *cart_create(size_t id);

/// @brief Inserts a cart into the store and advances the next cart id.
/// @param store The store to insert into.
/// @param cart The cart to insert. The store takes ownership.
/// @note Exists so the frontend does not have to know how the store keys
///       its carts. Also increments next_cart_id, which otherwise stays
///       at the same value.
void add_cart_to_store(store_t *store, cart_t *cart);

/// @brief Sums how many units of an item all carts together plan to buy.
/// @param store The store whose carts to walk.
/// @param merch The item to count.
/// @return The total quantity of merch across every cart in the store.
/// @note Walks every cart and every item in it, so it is O(carts * items).
size_t get_occupied_stock(store_t *store, merch_t *merch);

/// @brief Adds a quantity of an item to a cart.
/// @param store The store holding the cart and the item.
/// @param id The cart's id.
/// @param name The item's name.
/// @param quantity How many units to add. Must fit the available stock.
/// @return true if the item was added, false if the cart or the item does
///         not exist, or if quantity is larger than what is available.
/// @note available stock is total stock minus what other carts already
///       hold, so two carts cannot promise the same units.
bool add_to_cart(store_t *store, size_t id, char *name, size_t quantity);

/// @brief Removes a cart from the store.
/// @param store The store to remove from.
/// @param id The cart's id.
/// @return true if the cart was found and removed, false otherwise.
/// @note The cart is freed through the store's cart_destroy_callback.
bool remove_cart(store_t *store, size_t id);

/// @brief Removes a quantity of an item from a cart.
/// @param store The store holding the cart.
/// @param id The cart's id.
/// @param name The item's name.
/// @param quantity How many units to remove.
/// @return true if the amount was removed, false if the cart or the item
///         is missing, or if quantity is larger than the cart holds.
/// @note Removing the last unit removes the item from the cart entirely.
bool remove_from_cart(store_t *store, size_t id, char *name, size_t quantity);

/// @brief Counts what a cart costs, price times quantity for each item.
/// @param store The store to read prices from.
/// @param id The cart's id.
/// @return The total cost in ore, or 0 if the cart does not exist or is empty.
/// @note The two cases cannot be told apart from the return value.
int calculate_cost(store_t *store, size_t id);

/// @brief Reduces the store's stock by what a cart holds.
/// @param store The store to decrease.
/// @param items_of_cart The cart's items table.
/// @note Removes a shelf from the stock table when it is emptied, and
///       takes from the next shelf when one shelf does not cover the amount.
void stock_decrease_merch(store_t *store, ioopm_hash_table_t *items_of_cart); 

/// @brief Checks that every item in a cart still exists and is in stock.
/// @param store The store to check against.
/// @param items_of_cart The cart's items table.
/// @return true if every item is valid, false otherwise.
/// @note Removes the offending item from the cart before returning false.
bool check_if_all_items_valid(store_t *store, ioopm_hash_table_t *items_of_cart);

/// @brief Checks out a cart.
/// @param store The store holding the cart.
/// @param id The cart's id.
/// @return true if the checkout succeeded, false if the cart does not
///         exist, or if any item in it is gone from the store or
///         holds more units than the stock allows.
/// @note On success the stock is reduced and the cart is removed. On
///       failure the cart stays in the store, minus the item that
///       made it invalid.
bool checkout(store_t *store, size_t id);