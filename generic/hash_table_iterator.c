#include "hash_table.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

typedef struct hash_table_iterator ioopm_hash_table_iterator_t;

struct hash_table_iterator
{
  ioopm_hash_table_t *ht;
  size_t current_bucket;
  entry_t *current_entry;
};

/// @brief Moves the hash table iterator to point at the next entry,
///        if there are no more entries in the bucket, the iterator moves on to the next bucket.
/// @param it Pointer to a hash_table_iterator
/// @return void
static void advance_iterator_state(ioopm_hash_table_iterator_t *it)
{
  // Check if bucket it empty
  if (it->current_entry != NULL)
  {
    it->current_entry = it->current_entry->next;
  }

  // if it was null advance to the next bucket
  if (it->current_entry == NULL)
  {
    it->current_bucket += 1;

    // if the next bucket exists, update the current entry
    if (it->current_bucket != it->ht->no_buckets)
    {
      it->current_entry = *(&it->ht->buckets[it->current_bucket]);
    }
  }
}

/**
 * @brief Advances the iterator past empty buckets.
 * Continues advancing the iterator while the current bucket is within
 * the hash table and contains no entry.
 * @param it Pointer to the hash table iterator to update.
 * @return void
 */
static void skip_empty_buckets(ioopm_hash_table_iterator_t *it)
{
  while (it->current_bucket != it->ht->no_buckets &&
         it->current_entry == NULL)
        {
          advance_iterator_state(it);
        }
}

ioopm_hash_table_iterator_t *ioopm_hash_table_iterator_create(ioopm_hash_table_t *ht)
{
  ioopm_hash_table_iterator_t *it = calloc(1, sizeof(ioopm_hash_table_iterator_t));
  it->ht = ht;
  it->current_bucket = 0;
  it->current_entry = *(&ht->buckets[0]);
  skip_empty_buckets(it);
  return it;
}

void ioopm_hash_table_iterator_destroy(ioopm_hash_table_iterator_t *it)
{
  free(it);
}

bool ioopm_hash_table_iterator_at_end(ioopm_hash_table_iterator_t *it)
{
  return it->current_bucket == it->ht->no_buckets;
}

elem_t ioopm_hash_table_iterator_current_key(ioopm_hash_table_iterator_t *it)
{
  return it->current_entry->key;
}

elem_t ioopm_hash_table_iterator_current_value(ioopm_hash_table_iterator_t *it)
{
  return it->current_entry->value;
}

void ioopm_hash_table_iterator_advance(ioopm_hash_table_iterator_t *it)
{
  advance_iterator_state(it);
  skip_empty_buckets(it);
}
