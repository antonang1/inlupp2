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

merch_t *merch_create(char *name, char *desc, size_t price);

void merch_add_to_store(store_t *store, merch_t *merch);

bool merch_remove_from_store(store_t *store, merch_t *merch);

bool merch_edit(store_t *store, char *old_name, char *new_name, char *new_desc, size_t new_price);