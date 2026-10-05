#include <CUnit/Basic.h>
#include "hash_table.h"
#include "hash_table_iterator.h"
#include "common.h"

int init_suite(void) {
  // Change this function if you want to do something *before* you
  // run a test suite
  return 0;
}

int clean_suite(void) {
  // Change this function if you want to do something *after* you
  // run a test suite
  return 0;
}

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

void test_create_destroy()
{
   ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);

   CU_ASSERT_PTR_NOT_NULL(ht);
   ioopm_hash_table_destroy(ht);
}

void test_update_key() // från instruktionerna
{
  // create new hash table
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);

  elem_t key;
  key.s = "abc";
  elem_t value = int_elem(123);
  elem_t value2 = int_elem(456);
  // check that key is not in ht
  elem_t result = int_elem(0);
  
  CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, key, &result));
  CU_ASSERT_EQUAL(result.i, 0);

  // insert key-value pair and check that the mapping exists
  ioopm_hash_table_insert(ht, key, value);
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(ht, key, &result));
  CU_ASSERT_EQUAL(result.i, value.i);


  // insert key med nytt värde
  ioopm_hash_table_insert(ht, key, value2);
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(ht, key, &result));
  CU_ASSERT_EQUAL(result.i, value2.i);


  // destroy hash table
  ioopm_hash_table_destroy(ht);
}

void test_insert_once() // från instruktionerna
{
  // create new hash table
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);


  elem_t key;
  key.s = "abc";
  elem_t value = int_elem(123);

  // check that key is not in ht
  elem_t result = int_elem(0);
  CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, key, &result));
  CU_ASSERT_EQUAL(result.i, 0);

  // insert key-value pair and check that the mapping exists
  ioopm_hash_table_insert(ht, key, value);
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(ht, key, &result));
  CU_ASSERT_EQUAL(result.i, value.i);

  // destroy hash table
  ioopm_hash_table_destroy(ht);
}

// This test case was written with assistance from ChatGPT.
void test_remove_existing_key()
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);


    elem_t key;
    key.s = "abc";
    elem_t value = int_elem(123);
    elem_t result = int_elem(0);

    ioopm_hash_table_insert(ht, key, value);

    // Remove the key
    CU_ASSERT_TRUE(ioopm_hash_table_remove(ht, key, &result));

    // Check that the removed value was returned
    CU_ASSERT_EQUAL(result.i, value.i);

    // Check that the key is no longer in the table
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, key, &result));

    ioopm_hash_table_destroy(ht);
}

// This test case was written with assistance from ChatGPT.
void test_remove_nonexisting_key()
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);


    elem_t key;
    key.s = "A*";
    elem_t result = int_elem(999);

    // Nothing has been inserted, so removal should fail
    CU_ASSERT_FALSE(ioopm_hash_table_remove(ht, key, &result));

    // The result should not have been changed
    CU_ASSERT_EQUAL(result.i, int_elem(999).i);

    ioopm_hash_table_destroy(ht);
}

// This test case was written with assistance from ChatGPT.
void test_remove_only_key()
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);


    elem_t key;
    key.s = "abc";
    elem_t value = int_elem(123);
    elem_t result = int_elem(0);

    ioopm_hash_table_insert(ht, key, value);

    CU_ASSERT_TRUE(ioopm_hash_table_remove(ht, key, &result));
    CU_ASSERT_EQUAL(result.i, value.i);

    // The table should now behave as if the key was never inserted
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, key, &result));

    // Removing it again should fail
    CU_ASSERT_FALSE(ioopm_hash_table_remove(ht, key, &result));

    ioopm_hash_table_destroy(ht);
}

// This test case was written with assistance from ChatGPT.
void test_remove_from_collision_chain()
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);


    /*
     * We need keys that hash to the same bucket.
     * These values can be replaced with known colliding keys
     * if your assignment provides them.
     *
     * For now, find two/three keys that collide with your hash
     * function.
     */

     elem_t key1;
     key1.s = "A*";    
     elem_t key2;
     key2.s = "B-";

    elem_t value1 = int_elem(100);
    elem_t value2 = int_elem(200);
    elem_t result = int_elem(0);

    ioopm_hash_table_insert(ht, key1, value1);
    ioopm_hash_table_insert(ht, key2, value2);

    /*
     * Remove key1 and make sure key2 is still there.
     *
     * This specifically tests that:
     *
     * previous->next = current->next;
     *
     * correctly reconnects the linked list.
     */
    CU_ASSERT_TRUE(ioopm_hash_table_remove(ht, key1, &result));
    CU_ASSERT_EQUAL(result.i, value1.i);

    CU_ASSERT_TRUE(ioopm_hash_table_lookup(ht, key2, &result));
    CU_ASSERT_EQUAL(result.i, value2.i);

    CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, key1, &result));

    ioopm_hash_table_destroy(ht);
}

// This test case was written with assistance from ChatGPT.
void test_remove_last_entry_in_collision_chain()
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);


    elem_t key1 = string_elem("A*");
    elem_t key2 = string_elem("B-");  

    elem_t value1 = int_elem(100);
    elem_t value2 = int_elem(200);
    elem_t result = int_elem(0);

    ioopm_hash_table_insert(ht, key1, value1);
    ioopm_hash_table_insert(ht, key2, value2);

    // Remove the second/last entry
    CU_ASSERT_TRUE(ioopm_hash_table_remove(ht, key2, &result));
    CU_ASSERT_EQUAL(result.i, value2.i);

    // key1 should still exist
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(ht, key1, &result));
    CU_ASSERT_EQUAL(result.i, value1.i);

    // key2 should be gone
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, key2, &result));

    ioopm_hash_table_destroy(ht);
}

// 1. Tom tabell ska inte ha nyckeln k
//Gjorda av deepseek
static void test_has_key_empty(void) {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);


  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, string_elem("A*")));

  ioopm_hash_table_destroy(ht);
}
// 2. Efter insert av k ska k finnas, men inte k2
//Gjorda av deepseek
static void test_has_key_single(void) {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);

  ioopm_hash_table_insert(ht, string_elem("k"), int_elem(1));
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, string_elem("k")));
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, string_elem("B-")));

  ioopm_hash_table_destroy(ht);
}

// 3. Efter insert av k1, k2, k3 ska alla finnas, men inte k4
//Gjorda av deepseek
static void test_has_key_multiple(void) {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);

  ioopm_hash_table_insert(ht, string_elem("A*"), int_elem(1));
  ioopm_hash_table_insert(ht, string_elem("B-"), int_elem(2));
  ioopm_hash_table_insert(ht, string_elem("C0"), int_elem(3));

  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, string_elem("A*")));
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, string_elem("B-")));
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, string_elem("C0")));
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, string_elem("HEJ")));

  ioopm_hash_table_destroy(ht);
}

// 4. Insert + remove av k => k ska inte finnas
//Gjorda av deepseek
static void test_remove_single(void) {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);

  elem_t result = int_elem(0);
  ioopm_hash_table_insert(ht, string_elem("k"), int_elem(42));
  CU_ASSERT_TRUE(ioopm_hash_table_remove(ht, string_elem("k"), &result));
  CU_ASSERT_EQUAL(result.i, 42);
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, string_elem("k")));

  ioopm_hash_table_destroy(ht);
}

// 5. Insert k1,k2,k3, remove k2 => k1,k3 finns, k2 inte
//Gjorda av deepseek
static void test_remove_middle(void) {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);

  elem_t result = int_elem(0);
  ioopm_hash_table_insert(ht, string_elem("A*"), int_elem(1));
  ioopm_hash_table_insert(ht, string_elem("B-"), int_elem(2));
  ioopm_hash_table_insert(ht, string_elem("C0"), int_elem(3));

  CU_ASSERT_TRUE(ioopm_hash_table_remove(ht, string_elem("B-"), &result));
  CU_ASSERT_EQUAL(result.i, 2);

  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, string_elem("A*")));
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, string_elem("C0")));
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, string_elem("B-")));

  ioopm_hash_table_destroy(ht);
}

// size of an empty hash table
// gjort av deepseek
static void test_size_empty(void) {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 0);

  ioopm_hash_table_destroy(ht);
}

// size of a singleton hash table
// gjort av deepseek
static void test_size_singleton(void) {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);

  ioopm_hash_table_insert(ht, string_elem("k"), int_elem(1));
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 1);

  ioopm_hash_table_destroy(ht);
}

// size of a larger hash table
// gjort av deepseek
static void test_size_larger(void) {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);

  ioopm_hash_table_insert(ht, string_elem("k1"), int_elem(1));
  ioopm_hash_table_insert(ht, string_elem("k2"), int_elem(2));
  ioopm_hash_table_insert(ht, string_elem("k3"), int_elem(3));
  ioopm_hash_table_insert(ht, string_elem("k4"), int_elem(4));
  ioopm_hash_table_insert(ht, string_elem("k5"), int_elem(5));
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 5);
  
  ioopm_hash_table_destroy(ht);
}

// size after removing an element
// gjort av deepseek
static void test_size_after_remove(void) {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);

  elem_t result = int_elem(0);
  
  ioopm_hash_table_insert(ht, string_elem("A*"), int_elem(1));
  ioopm_hash_table_insert(ht, string_elem("B-"), int_elem(2));
  ioopm_hash_table_insert(ht, string_elem("C0"), int_elem(3));
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 3);

  CU_ASSERT_TRUE(ioopm_hash_table_remove(ht, string_elem("A*"), &result));
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 2);

  CU_ASSERT_TRUE(ioopm_hash_table_remove(ht, string_elem("B-"), &result));
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 1);

  CU_ASSERT_TRUE(ioopm_hash_table_remove(ht, string_elem("C0"), &result));
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 0);

  ioopm_hash_table_destroy(ht);
}


int main() {
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite my_test_suite = CU_add_suite("My awesome test suite", init_suite, clean_suite);
  if (my_test_suite == NULL) {
      // If the test suite could not be added, tear down CUnit and exit
      CU_cleanup_registry();
      return CU_get_error();
  }

  // This is where we add the test functions to our test suite.
  // For each call to CU_add_test we specify the test suite, the
  // name or description of the test, and the function that runs
  // the test in question. If you want to add another test, just
  // copy a line below and change the information
  if ( CU_add_test(my_test_suite, "hashtable create / destroy", test_create_destroy) == NULL 
      || CU_add_test(my_test_suite, "hashtable insert / lookup / destroy", test_insert_once) == NULL
      || CU_add_test(my_test_suite, "insert same key twice with different values", test_update_key) == NULL
      || CU_add_test(my_test_suite, "remove existing key", test_remove_existing_key) == NULL
      || CU_add_test(my_test_suite, "remove nonexisting key", test_remove_nonexisting_key) == NULL
      || CU_add_test(my_test_suite, "remove only key", test_remove_only_key) == NULL
      || CU_add_test(my_test_suite, "remove from collision chain", test_remove_from_collision_chain) == NULL
      || CU_add_test(my_test_suite, "remove last entry in collision chain", test_remove_last_entry_in_collision_chain) == NULL
      || CU_add_test(my_test_suite, "tries to find key that does not exist", test_has_key_empty) == NULL
      || CU_add_test(my_test_suite, "Finds valid key, does not find invalid key", test_has_key_single) == NULL
      || CU_add_test(my_test_suite, "Finds mult valid keys, but not one invalid", test_has_key_multiple) == NULL
      || CU_add_test(my_test_suite, "Inserts key, then removes it", test_remove_single) == NULL
      || CU_add_test(my_test_suite, "Inserts keys, removes middle key", test_remove_middle) == NULL
      || CU_add_test(my_test_suite, "size of empty table", test_size_empty) == NULL
      || CU_add_test(my_test_suite, "size of singleton table", test_size_singleton) == NULL
      || CU_add_test(my_test_suite, "size of larger table", test_size_larger) == NULL
      || CU_add_test(my_test_suite, "size after removing element", test_size_after_remove) == NULL
  )
    {
      // If adding any of the tests fails, we tear down CUnit and exit
      CU_cleanup_registry();
      return CU_get_error();
    }

  // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
  // Use CU_BRM_NORMAL to only print errors and a summary
  CU_basic_set_mode(CU_BRM_NORMAL);
  
  // This is where the tests are actually run!
  CU_basic_run_tests();

  // Tear down CUnit before exiting
  CU_cleanup_registry();
  return CU_get_error();
}