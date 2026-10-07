#pragma once

#include "../data_structures/hash_table.h"

typedef struct store store_t;

struct store
{
    ioopm_hash_table_t *merchandise;
    ioopm_hash_table_t *stock;
    ioopm_hash_table_t *carts;
    size_t next_cart_id;
};

