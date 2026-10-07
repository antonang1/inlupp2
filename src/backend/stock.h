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
