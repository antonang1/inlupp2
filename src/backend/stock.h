#pragma once


#include <stddef.h>
#include <stdbool.h>

#include "../data_structures/list_iterator.h"

/* 
NOTE: Documentation outline generated with KLAR, then rewritten and verified by Signe von Greiff and Anton Äng.
No code in this repository was generated.
*/

typedef struct store store_t;
typedef struct merch merch_t;
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
int ioopm_comparison_shelf(elem_t a, elem_t b);

/// @brief  Creates a shelf holding a quantity of an item.
/// @param shelf The shelf's name, for example "A12".
/// @param merch_name The name of the item stored on the shelf.
/// @return A new shelf. The caller sets the quantity afterwards.
/// @note Allocates the struct and copies both strings.
///       Free everything with the store's destroy function.
shelf_t *ioopm_shelf_create(const char *shelf, const char *merch_name);

/// @brief Adds stock to a shelf, or creates the shelf if it does not exist.
/// @param store The store to replenish.
/// @param shelf_name The name of the shelf, for example "A12".
/// @param merch_name The item that the shelf holds.
/// @param quantity How many units to add. Must be at least 1.
/// @return true if stock was added, false if quantity is 0, if the
///         item is not in the store, or if the shelf already holds
///         a different item.
/// @note If the shelf is new, the store takes ownership of it
bool ioopm_replenish_stock(store_t *store, char *shelf_name, char *merch_name, size_t quantity);

/// @brief Removes an item's shelves from the store's stock table.
/// @param store The store to remove the shelves from.
/// @param merch_to_clear The item whose shelves are to be removed.
/// @return true if every shelf was found and removed, false as soon as
///         one shelf could not be removed.
/// @note Removes the shelves from the stock table only. The list inside
///       the item is not touched, so freeing it stays the caller's job.
bool ioopm_stock_remove_merch(store_t *store, merch_t *merch_to_clear);

/// @brief Writes the item's current name onto every shelf that holds it.
/// @param edited_merch The item whose name was changed. Its shelfs list is
///                     traversed and each shelf's merch_name is replaced.
/// @note Frees the old merch_name string and strdups the new one, so each
///       shelf owns its own copy.
/// @note Requires edited_merch->shelfs to be non-NULL. It is called from
///       merch_edit, which already checks for that, but the function does
///       not check on its own.
void ioopm_stock_update_merch_name(merch_t *edited_merch);

/// @brief Returns all shelves holding a given item, sorted by shelf name.
/// @param merch The item whose shelves to list.
/// @param size_out Written with the number of shelves returned.
/// @return A heap-allocated array of elem_t holding shelf pointers,
///         or an empty array if the item has no shelves.
/// @note The array is owned by the caller, who must free it. 
///       The shelves themselves belong to the store and must not be freed.
elem_t *ioopm_list_shelves(merch_t *merch, size_t *size_out);

/// @brief Returns the total number of units of an item, across all its shelves.
/// @param merch The item whose shelves to sum.
/// @return The sum of the quantity on every shelf holding the item,
///         or 0 if the item has no shelves.
/// @note Reads the shelves through the item's own list, not through the
///       store's stock table, so no lookup is needed.
size_t ioopm_get_total_stock(merch_t *merch);
