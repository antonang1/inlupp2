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


int comparison_shelf(elem_t a, elem_t b);

shelf_t *shelf_create(const char *shelf, const char *merch_name);

bool replenish_stock(store_t *store, char *shelf_name, char *merch_name, size_t quantity);

bool stock_remove_merch(store_t *store, merch_t *merch_to_clear);

//elem_t *list_shelves(merch_t *merch, size_t *size_out);
