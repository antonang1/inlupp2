#pragma once
#include "common.h"

/**
* @file hash_table.h
* @author Elvin Porter och Anton Äng
* @date 14/09-2026
* @brief Simple hash table that maps string keys to integer values.
*
* Here typically goes a more extensive explanation of what the header
* defines. Doxygens tags are words preceeded by either a backslash @\
* or by an at symbol @@.
*
*/

/// @brief Create a new hash table
/// @param hash_fn pointer to hash function 
/// @param key_eq_fn pointer to equality function 
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function *hash_fn, ioopm_eq_function *key_eq_fn);

/// @brief Delete a hash table and free its memory
/// @param ht a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value);

/// @brief lookup value for key in hash table ht and writes it to result
/// @param ht hash table operated upon
/// @param key key to lookup
/// @param result pointer to where the result gets written
/// @return true/false depending on wether lookup was sucessfull    
bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key, elem_t *result);

/// @brief Removes the mapping from a key to a value and stores the removed value in result.
/// @param ht hash table operated upon
/// @param key key to remove
/// @param result variable to store removed value in.
/// @return true/false depending on wether lookup was sucessfull
bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key, elem_t *result);

/// @brief Checks if the key exists in the given hashtable 
/// @param ht Hashtable to check
/// @param key key to check
/// @return True if key is in hashtable
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key);

/// @brief Checks wether the hashtable has any entries
/// @param ht hashtable
/// @return True if size is 0
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht);

/// @brief fetches the amount of entries in the given hashtable
/// @param ht hashtable
/// @return size of the hashtable as a positive integer
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht);
