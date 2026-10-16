#pragma once

#include "merch.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>
#include "../data_structures/hash_table.h"

typedef struct shelf shelf_t;

struct shelf
{
    char *shelf_name;
    char *merch_name;
    size_t quantity;
};


/// @brief Compares two shelves by shelf name, for sorting.
/// @param a An elem_t holding a shelf_t pointer.
/// @param b An elem_t holding a shelf_t pointer.
/// @return Negative if a sorts before b, zero if the names are equal,
///         positive if a sorts after b. Returns zero if either is NULL.
int comparison_shelf(elem_t a, elem_t b);

/// @brief  Creates a shelf holding a quantity of an item.
/// @param shelf The shelf's name, for example "A12".
/// @param merch_name The name of the item stored on the shelf.
/// @return A new shelf. The caller sets the quantity afterwards.
/// @note Allocates the struct and copies both strings.
///       Free everything with the store's destroy function.
shelf_t *shelf_create(const char *shelf, const char *merch_name);

/// @brief Adds stock to a shelf, or creates the shelf if it does not exist.
/// @param store The store to replenish.
/// @param shelf_name The name of the shelf, for example "A12".
/// @param merch_name The item that the shelf holds.
/// @param quantity How many units to add. Must be at least 1.
/// @return true if stock was added, false if quantity is 0, if the
///         item is not in the store, or if the shelf already holds
///         a different item.
/// @note If the shelf is new, the store takes ownership of it
bool replenish_stock(store_t *store, char *shelf_name, char *merch_name, size_t quantity);

bool stock_remove_merch(store_t *store, merch_t *merch_to_clear);

/// @brief Returns all shelves holding a given item, sorted by shelf name.
/// @param merch The item whose shelves to list.
/// @param size_out Written with the number of shelves returned.
/// @return A heap-allocated array of elem_t holding shelf pointers,
///         or an empty array if the item has no shelves.
/// @note The array is owned by the caller, who must free it. 
///       The shelves themselves belong to the store and must not be freed.
elem_t *list_shelves(merch_t *merch, size_t *size_out);
