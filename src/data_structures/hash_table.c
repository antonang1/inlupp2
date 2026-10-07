#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "common.h"

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function *hash_fn, ioopm_eq_function *key_eq_fn)
{
  ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));
  ht->no_buckets = 17;
  ht->buckets = calloc(ht->no_buckets, sizeof(entry_t *));
  ht->ht_size = 0;
  ht->load_factor = 0.5;
  ht->hash = hash_fn;
  ht->is_equal = key_eq_fn;

  return ht;
}

/// @brief Frees allocated memory of an entry_t
/// @pre current is allocated in memory before this function is called
/// @param current A pointer to the entry_t that will be freed
/// @return void
static void entry_destroy(entry_t *current)
{
  free(current);
}

/// @brief Frees the allocated memory of a bucket in a hash table
/// @param e pointer to a entry_t
/// @return void
static void free_bucket(entry_t **current_entry)
{
  while (*current_entry != NULL) // traverse the bucket from current to the last non-NULL entry
  {
    entry_t *next = (*current_entry)->next;
    entry_destroy(*current_entry);
    *current_entry = next; // update current pointer to next entry_t
  }
}

/**
 * @brief Finds the link to the entry associated with a given key.
 * Calculates the bucket corresponding to the key and traverses the
 * bucket's linked list until the entry with the key is found or the
 * end of the list is reached. The returned pointer can be used both
 * to access the found entry and to modify the link pointing to it.
 * @param ht The hash table to search.
 * @param key The key to search for.
 * @return A pointer to the link pointing to the entry with the given key,
 *         or to the NULL pointer at the end of the bucket if the key
 *         is not found.
 */
static entry_t **find_entry_link(ioopm_hash_table_t *ht, elem_t key)
{
  size_t bucket = ht->hash(key) % ht->no_buckets;
  entry_t **link = &ht->buckets[bucket];

  while (*link != NULL && !(ht->is_equal((*link)->key, key)))
  {
    link = &(*link)->next;
  }
  return link;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
  for (size_t index = 0; index < ht->no_buckets; index++)
  {
    entry_t **entry = &ht->buckets[index];
    free_bucket(entry);
  }
  free(ht->buckets);
  free(ht); // when all buckets only contains sentinel nodes, free ht
}

/// @brief Creates and returns an entry from the inputs
/// @param key Key for hashing the entry
/// @param value Value associated with the entry
/// @return Entry with pointer to NULL
static entry_t *entry_create(elem_t key, elem_t value)
{
  entry_t *next;
  next = calloc(1, sizeof(entry_t));
  next->key = key;
  next->value = value;

  return next;
}

/**
 * @brief Finds the link to an entry in a bucket array during rehashing.
 *
 * Calculates the bucket corresponding to the key and traverses the
 * bucket's linked list until an entry with the given key is found or
 * the end of the list is reached. The returned pointer points to the
 * link leading to the matching entry, or to the NULL pointer where a
 * new entry can be inserted.
 *
 * @param ht The hash table being rehashed, used for the hash and equality functions.
 * @param buckets The bucket array in which to search.
 * @param key The key to search for.
 *
 * @return A pointer to the link pointing to the matching entry, or to
 *         the NULL pointer at the end of the bucket if the key is not found.
 */
static entry_t **find_entry_link_rehash(ioopm_hash_table_t *ht, entry_t **buckets, elem_t key)
{
  size_t bucket = ht->hash(key) % ht->no_buckets;
  entry_t **link = &buckets[bucket];

  while (*link != NULL && !(ht->is_equal((*link)->key, key)))
  {
    link = &(*link)->next;
  }
  return link;
}

/**
 * @brief Inserts an entry into the new bucket array during rehashing.
 * @param ht The hash table being rehashed.
 * @param buckets The new bucket array.
 * @param key The key to insert.
 * @param value The value associated with the key.
 */
static void rehash_insert(ioopm_hash_table_t *ht, entry_t **buckets, elem_t key, elem_t value)
{
  entry_t **link = find_entry_link_rehash(ht, buckets, key);

  // if the key exists, update the value, otherwise create a new entry
  if (*link != NULL)
  {
    (*link)->value = value;
  }
  else
  {
    *link = entry_create(key, value);
  }
}

/**
 * @brief Rehashes all entries in a single bucket.
 * @param ht The hash table being rehashed.
 * @param new_buckets The new bucket array.
 * @param current_bucket The bucket currently being rehashed.
 */
static void rehash_bucket(ioopm_hash_table_t *ht, entry_t **new_buckets, entry_t **current_bucket)
{
  while (*current_bucket != NULL) // traverse the bucket from current to the last non-NULL entry
  {
    entry_t *next = (*current_bucket)->next;

    rehash_insert(ht, new_buckets, (*current_bucket)->key, (*current_bucket)->value);
    entry_destroy(*current_bucket);
    *current_bucket = next; // update current pointer to next entry_t
  }
}

/**
 * @brief Rehashes all buckets into a new bucket array.
 * @param ht The hash table being rehashed.
 * @param new_buckets The new bucket array.
 * @param old_buckets The number of buckets in the old array.
 */
static void rehash(ioopm_hash_table_t *ht, entry_t **new_buckets, size_t old_buckets)
{
  for (size_t index = 0; index < old_buckets; index++)
  {
    entry_t **current_bucket = &ht->buckets[index];
    rehash_bucket(ht, new_buckets, current_bucket);
  }
}

/**
 * @brief Resizes the hash table and rehashes its entries.
 * @param ht The hash table to resize.
 */
static void resize_table(ioopm_hash_table_t *ht)
{
  size_t primes[] = {17, 31, 67, 127, 257, 509, 1021, 2053, 4099, 8191, 16381};
  size_t required_capacity = (ht->ht_size) / ht->load_factor;

  for (int i = 0; i < 11; i++)
  {
    if (required_capacity < primes[i])
    {
      size_t old_no_buckets = ht->no_buckets;
      ht->no_buckets = primes[i]; //updaterar mängden buckets
      entry_t **new_buckets = calloc(ht->no_buckets, sizeof(entry_t *)); //allokerar minne för dem
      rehash(ht, new_buckets, old_no_buckets); //indexerar in alla entries i nya buckets
      free(ht->buckets);
      ht->buckets = new_buckets;
      return;
    }
  }

  return;
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{
  entry_t **link = find_entry_link(ht, key);

  // if the key exists, update the value, otherwise create a new entry
  if (*link != NULL)
  {
    (*link)->value = value;
  }
  else
  {
    (*link) = entry_create(key, value);

    (ht->ht_size)++; // increment ht_size when entry added.
    if ((float)ht->ht_size / ht->no_buckets > ht->load_factor)
    {
      resize_table(ht);
    }  
  }
}

bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key, elem_t *result)
{
  entry_t **link = find_entry_link(ht, key);

  if (*link == NULL) // if current is a NULL-entry, there is nothing to remove
  {
    return false;
  }
  else
  {
    entry_t *current = *link;
    *link = current->next;
    *result = current->value;

    entry_destroy(current);
    (ht->ht_size)--;
    
    return true;
  }
}

bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key, elem_t *result)
{

  entry_t **link = find_entry_link(ht, key);

  // if the key exists, return the value, otherwise, indicate that the lookup failed
  if (*link != NULL)
  {
    *result = (*link)->value;
    return true;
  }
  else
  {
    return false;
  }
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
  // function uses same logic as lookup, therefore conveniant to reuse it.

  elem_t tmp; // this is a filler, not important for has_key but needed for lookup.

  return ioopm_hash_table_lookup(ht, key, &tmp);
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
  return ht->ht_size;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht)
{
  return ioopm_hash_table_size(ht) == 0;
}
