#include <CUnit/Basic.h>
#include "../backend/merch.h"
#include "../backend/store.h"
#include "../backend/stock.h"
#include <stdbool.h>
#include <stdlib.h>

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

// --------------TESTS merch_create--------------
void test_create_merch() {
    char *merch_name = "TestItem";
    char *merch_desc = "TestDesc";
    size_t price = 1;

    merch_t *new_merch = merch_create(merch_name, merch_desc, price);

    CU_ASSERT_PTR_NOT_NULL(new_merch);
    CU_ASSERT_PTR_NOT_NULL(new_merch->name);
    CU_ASSERT_PTR_NOT_NULL(new_merch->desc);
    CU_ASSERT_EQUAL(new_merch->price, price);
    CU_ASSERT_PTR_NULL(new_merch->shelfs);

    merch_destroy(new_merch);

    // also test destroying a NULL merch_t
    merch_t *new_merch2 = calloc(1, sizeof(merch_t));
    CU_ASSERT_PTR_NULL(new_merch2->name);
    CU_ASSERT_PTR_NULL(new_merch2->desc);
    CU_ASSERT_EQUAL(new_merch2->price, 0);
    CU_ASSERT_PTR_NULL(new_merch2->shelfs);

    merch_destroy(new_merch2);

    return;
}

// --------------TESTS merch_add_to_store--------------
void test_add_merch()
{
    store_t *store = store_create();
    
    char *merch_name = "TestItem";
    char *merch_desc = "TestDesc";
    size_t price = 1;

    merch_add_to_store(store, merch_name, merch_desc, price);

    elem_t tmp;

    // test valid key
    bool found = ioopm_hash_table_lookup(store->merchandise, string_elem(merch_name), &tmp);
    CU_ASSERT_TRUE(found);
    
    // TestItem should not have any storage locations
    merch_t *merch_of_intrest = tmp.p;
    CU_ASSERT_PTR_NULL(merch_of_intrest->shelfs);

    //test invalid key
    char *nonexisting_merch_name = "TestItem2";
    found = ioopm_hash_table_lookup(store->merchandise, string_elem(nonexisting_merch_name), &tmp);
    CU_ASSERT_FALSE(found);

    //test adding merch to nonexisting store
    
    store_destroy(store);

    return;
}

void test_add_mult_merch()
{
  store_t *store = store_create();
    
  // add merch to store:
  char *merch_name1 = "TestItem";
  char *merch_desc1 = "TestDesc";
  size_t price1 = 1;
  merch_add_to_store(store, merch_name1, merch_desc1, price1);

  char *merch_name2 = "CoffeeMug";
  char *merch_desc2 = "Its a coffee mug";
  size_t price2 = 10;
  merch_add_to_store(store, merch_name2, merch_desc2, price2);

  char *merch_name3 = "TShirt";
  char *merch_desc3 = "Its a t-shirt";
  size_t price3 = 25;
  merch_add_to_store(store, merch_name3, merch_desc3, price3);

  // verify that we can retrive all of them independently:
  elem_t merch_1;
  bool found_1 = ioopm_hash_table_lookup(store->merchandise, string_elem(merch_name1), &merch_1);
  CU_ASSERT_TRUE(found_1);

  elem_t merch_2;
  bool found_2 = ioopm_hash_table_lookup(store->merchandise, string_elem(merch_name2), &merch_2);
  CU_ASSERT_TRUE(found_2);

  elem_t merch_3;
  bool found_3 = ioopm_hash_table_lookup(store->merchandise, string_elem(merch_name3), &merch_3);
  CU_ASSERT_TRUE(found_3);

  store_destroy(store);

  return;
}

void test_add_mult_merch_same_name()
{
  store_t *store = store_create();
    
  // add merch to store:
  char *merch_name1 = "TestItem";
  char *merch_desc1 = "TestDesc";
  size_t price1 = 1;

  bool added = merch_add_to_store(store, merch_name1, merch_desc1, price1);
  CU_ASSERT_TRUE(added);
  
  char *merch_name2 = "TestItem";
  char *merch_desc2 = "some other TestDesc";
  size_t price2 = 2;
  
  added = merch_add_to_store(store, merch_name2, merch_desc2, price2);
  CU_ASSERT_FALSE(added);

  //check that desc and price of merch was not changed
  elem_t tmp;
  ioopm_hash_table_lookup(store->merchandise, string_elem(merch_name1), &tmp);
  
  merch_t *merch_of_intrest = tmp.p;
  CU_ASSERT_STRING_EQUAL(merch_of_intrest->desc, merch_desc1);
  CU_ASSERT_EQUAL(merch_of_intrest->price, price1);

  store_destroy(store);
}

// --------------TESTS merch_remove_from_store--------------
void test_remove_single_merch()
{
  store_t *store = store_create();
  
  // add merch to store:
  char *merch_name1 = "TestItem";
  char *merch_desc1 = "TestDesc";
  size_t price1 = 1;
  
  bool added = merch_add_to_store(store, merch_name1, merch_desc1, price1);
  CU_ASSERT_TRUE(added);
  
  bool removed = merch_remove_from_store(store, merch_name1);
  CU_ASSERT_TRUE(removed);

  // try to access removed merch should not work
  elem_t merch;
  bool found = ioopm_hash_table_lookup(store->merchandise, string_elem(merch_name1), &merch);
  CU_ASSERT_FALSE(found);

  store_destroy(store);

}

void test_remove_nonexisting_merch()
{
  store_t *store = store_create();
    
  // add merch to store:
  char *merch_name1 = "TestItem";

  bool removed = merch_remove_from_store(store, merch_name1);
  CU_ASSERT_FALSE(removed);

  store_destroy(store);
}

// This test was created with assistance of ChatGPT (GPT-6)
void test_remove__merch_w_stock()
{
  store_t *store = store_create();
  
  char *merch_name1 = "TestItem";
  
  merch_add_to_store(store, merch_name1, "TestDesc", 1);
  
  replenish_stock(store, "A10", merch_name1, 7);

  bool removed1 = merch_remove_from_store(store, merch_name1);
  CU_ASSERT_TRUE(removed1);

  elem_t merch;
  CU_ASSERT_FALSE(ioopm_hash_table_lookup(
      store->merchandise, string_elem(merch_name1), &merch));

  CU_ASSERT_FALSE(ioopm_hash_table_lookup(store->stock, string_elem("A10"), &merch));

  store_destroy(store);
}

void test_remove_mult_merch()
{
  store_t *store = store_create();

  char *merch_name1 = "TestItem";
  char *merch_name2 = "CoffeeMug";
  char *merch_name3 = "TShirt";

  merch_add_to_store(store, merch_name1, "TestDesc", 1);
  merch_add_to_store(store, merch_name2, "Its a coffee mug", 10);
  merch_add_to_store(store, merch_name3, "Its a t-shirt", 25);
  
  bool removed1 = merch_remove_from_store(store, merch_name1);
  bool removed2 = merch_remove_from_store(store, merch_name2);
  bool removed3 = merch_remove_from_store(store, merch_name3);

  CU_ASSERT_TRUE(removed1);
  CU_ASSERT_TRUE(removed2);
  CU_ASSERT_TRUE(removed3);

  elem_t merch;
  CU_ASSERT_FALSE(ioopm_hash_table_lookup(
      store->merchandise, string_elem(merch_name1), &merch));
  CU_ASSERT_FALSE(ioopm_hash_table_lookup(
      store->merchandise, string_elem(merch_name2), &merch));
  CU_ASSERT_FALSE(ioopm_hash_table_lookup(
      store->merchandise, string_elem(merch_name3), &merch));

  store_destroy(store);
}


// --------------TESTS merch_edit--------------
void test_edit_desc()
{
  store_t *store = store_create();
  
  // add merch to store:
  char *merch_name1 = "TestItem";
  char *merch_desc1 = "TestDesc";
  size_t price1 = 1;
  
  bool added = merch_add_to_store(store, merch_name1, merch_desc1, price1);
  CU_ASSERT_TRUE(added);
  
  replenish_stock(store, "A25", merch_name1, 8);
  
  bool edit = merch_edit(store, merch_name1, "TestItem2", "NewDesc", 1);
  CU_ASSERT_TRUE(edit);

  elem_t merch;
  bool found = ioopm_hash_table_lookup(store->merchandise, string_elem("TestItem2"), &merch);
  CU_ASSERT_TRUE(found);

  merch_t *merch_of_intrest = merch.p;

  CU_ASSERT_STRING_EQUAL(merch_of_intrest->desc, "NewDesc");

  shelf_t *shelf = merch_of_intrest->shelfs->first->head.p;
  CU_ASSERT_STRING_EQUAL(shelf->merch_name, "TestItem2");

  store_destroy(store);
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
  // copy a line below and change the information'
  if ( CU_add_test(my_test_suite, "Corectly creation and removal of merch", test_create_merch) == NULL
       || CU_add_test(my_test_suite, "Add merch to store", test_add_merch) == NULL
       || CU_add_test(my_test_suite, "Add mult merch and retrive them one by one", test_add_mult_merch) == NULL
       || CU_add_test(my_test_suite, "Add mult merch with same name should be invalid behaviour", test_add_mult_merch_same_name) == NULL
       || CU_add_test(my_test_suite, "Add and remove single merch from store", test_remove_single_merch) == NULL
       || CU_add_test(my_test_suite, "Add and remove single merch w stock from store", test_remove__merch_w_stock) == NULL
       || CU_add_test(my_test_suite, "Removal of nonexisting merch should be invalid behaviour", test_remove_nonexisting_merch) == NULL
       || CU_add_test(my_test_suite, "Add and remove mult merch from store", test_remove_mult_merch) == NULL
       || CU_add_test(my_test_suite, "Add and edit merch once", test_edit_desc) == NULL
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