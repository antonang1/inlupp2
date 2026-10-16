#pragma once

#include "../data_structures/hash_table.h"
#include "../data_structures/linked_list.h"
#include "store.h"

typedef struct merch merch_t;

struct merch
{
    char *name;
    char *desc;
    size_t price;
    ioopm_list_t *shelfs;
};


/// @brief Returns the list of shelves holding an item.
/// @param merch The item to read from.
/// @return The linked list of shelf pointers. Owned by the item.
ioopm_list_t *merch_shelves(merch_t *merch);

/// @brief Returns the price of a merch in ore.
/// @param merch The merch to read the price from.
/// @return The price in ore.
int merch_price(merch_t *merch);

/// @brief Compares two merchandise items by name, for sorting.
/// @param a An elem_t holding a merch_t pointer.
/// @param b An elem_t holding a merch_t pointer.
/// @return A negative value if a sorts before b, zero if the names are 
///         equal, and a positive value if a sorts after b.
int comparison_merch(elem_t a, elem_t b);


void merch_destroy(merch_t *value);

/// @brief Creates a merchandise item from the given fields.
/// @param name The item's name.
/// @param desc The item's description. 
/// @param price The item's price in öre.
/// @return A new item, or NULL if allocation fails.
/// @note The caller owns the returned item and must free it with 
///       merch_destroy (which is private to the module).
merch_t *merch_create(const char *name, const char *desc, size_t price);

/// @brief Adds a new item to the store, creating it from the given fields.
/// @param store The store to add the item to.
/// @param name The name of the item.
/// @param desc The description of the item. 
/// @param price The price of the item in öre.
/// @return true if the item was created and added, false if an item 
///         with the same name already exists.
/// @note The store takes ownership of the created item.
bool merch_add_to_store(store_t *store, char *name, char *desc, size_t price);

/// @brief  Removes an item from the store and frees it.
/// @param store The store to remove the item from.
/// @param merch The item to remove, identified by its name.
/// @return true if the item was found and removed, false otherwise.
/// @note The item and its shelves list are destroyed. Any pointer
///       the caller still holds to it becomes invalid.
bool merch_remove_from_store(store_t *store, char *merch_name);

/// @brief Changes the name, description and price of an existing item.
/// @param store The store containing the item.
/// @param old_name The item's current name, used to find it.
/// @param new_name The new name.
/// @param new_desc The new description.
/// @param new_price The new price in öre.
/// @return  true if the item was found and edited, false if no item
///          had old_name, or if new_name is already taken.
/// @note The item is removed and re-inserted under the new name,
///       because its key changes.
bool merch_edit(store_t *store, char *old_name, char *new_name, char *new_desc, size_t new_price);

elem_t *list_merchandise(store_t *store, size_t *size_out);
