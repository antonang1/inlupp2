#pragma once

#include "../data_structures/hash_table.h"
#include "../data_structures/linked_list.h"
#include "store.h"


// Everything about one item, with no table in sight.

// merch_create(name, desc, price)
// merch_destroy(m)
// merch_eq(a, b)
// merch_print(m)


typedef struct merch merch_t;

struct merch
{
    char *name;
    char *desc;
    size_t price;
    ioopm_list_t *shelfs;
};

int comparison_merch(elem_t a, elem_t b);

void merch_destroy(merch_t *value);

merch_t *merch_create(const char *name, const char *desc, size_t price);

bool merch_add_to_store(store_t *store, char *name, char *desc, size_t price);

bool merch_remove_from_store(store_t *store, char *merch_name);

bool merch_edit(store_t *store, char *old_name, char *new_name, char *new_desc, size_t new_price);

elem_t *list_merchandise(store_t *store, size_t *size_out);
