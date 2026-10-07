#include "store.h"
#include "merch.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../data_structures/hash_table.h"

static size_t ioopm_string_knr_hash(elem_t key)
{
  const char *str = key.s;
  size_t result = 0;
  while (*str != '\0')
  {
    result = result * 31 + ((unsigned char) *str);
    str++;
  }
  return result;
}

static bool string_compare(elem_t str1, elem_t str2){
  const char *string1 = str1.s;
  const char *string2 = str2.s;

  return strcmp(string1, string2) == 0;
}
