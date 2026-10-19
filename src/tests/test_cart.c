#include <CUnit/Basic.h>
#include "../backend/cart.h"
#include "../backend/merch.h"
#include "../backend/store.h"
#include "../backend/stock.h"
#include "../data_structures/hash_table.h"
#include "../utils/utils.h"
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


// --------------TESTS store_create--------------
void test_store_create_has_carts()
{
    store_t *store = store_create();

    CU_ASSERT_PTR_NOT_NULL(store->carts);
    CU_ASSERT_TRUE(ioopm_hash_table_is_empty(store->carts));

    store_destroy(store);
}

// --------------TESTS cart_create--------------
void test_create_cart()
{
    cart_t *cart = cart_create(7);

    CU_ASSERT_PTR_NOT_NULL(cart);
    CU_ASSERT_EQUAL(cart->id, 7);
    CU_ASSERT_PTR_NOT_NULL(cart->items);
    CU_ASSERT_EQUAL(ioopm_hash_table_size(cart->items), 0);

    ioopm_hash_table_destroy(cart->items);
    free(cart);
}

// --------------TESTS add_to_cart--------------
void test_add_single_item()
{
    store_t *store = store_create();

    cart_t *cart = cart_create(1);
    ioopm_hash_table_insert(store->carts, int_elem(1), ptr_elem(cart));
    merch_add_to_store(store, "Apple", "Red apple", 5);

    bool added = add_to_cart(store, 1, "Apple", 3);
    CU_ASSERT_TRUE(added);

    elem_t value;
    bool found = ioopm_hash_table_lookup(cart->items, string_elem("Apple"), &value);
    CU_ASSERT_TRUE(found);
    CU_ASSERT_EQUAL(value.i, 3);

    store_destroy(store);
}

void test_add_same_item_twice_sums()
{
    store_t *store = store_create();

    cart_t *cart = cart_create(1);
    ioopm_hash_table_insert(store->carts, int_elem(1), ptr_elem(cart));
    merch_add_to_store(store, "Apple", "Red apple", 5);

    add_to_cart(store, 1, "Apple", 3);
    add_to_cart(store, 1, "Apple", 4);

    elem_t value;
    ioopm_hash_table_lookup(cart->items, string_elem("Apple"), &value);
    CU_ASSERT_EQUAL(value.i, 7);

    store_destroy(store);
}

void test_add_mult_items()
{
    store_t *store = store_create();

    cart_t *cart = cart_create(1);
    ioopm_hash_table_insert(store->carts, int_elem(1), ptr_elem(cart));

    merch_add_to_store(store, "Apple", "Red apple", 5);
    merch_add_to_store(store, "Pear", "Green pear", 10);
    merch_add_to_store(store, "Mug", "Coffee mug", 25);

    add_to_cart(store, 1, "Apple", 3);
    add_to_cart(store, 1, "Pear", 2);
    add_to_cart(store, 1, "Mug", 1);

    elem_t value;
    ioopm_hash_table_lookup(cart->items, string_elem("Apple"), &value);
    CU_ASSERT_EQUAL(value.i, 3);
    ioopm_hash_table_lookup(cart->items, string_elem("Pear"), &value);
    CU_ASSERT_EQUAL(value.i, 2);
    ioopm_hash_table_lookup(cart->items, string_elem("Mug"), &value);
    CU_ASSERT_EQUAL(value.i, 1);
    CU_ASSERT_EQUAL(ioopm_hash_table_size(cart->items), 3);

    store_destroy(store);
}

void test_add_to_nonexisting_cart()
{
    store_t *store = store_create();

    merch_add_to_store(store, "Apple", "Red apple", 5);

    bool added = add_to_cart(store, 99, "Apple", 1);
    CU_ASSERT_FALSE(added);

    store_destroy(store);
}

void test_add_nonexisting_merch()
{
    store_t *store = store_create();

    cart_t *cart = cart_create(1);
    ioopm_hash_table_insert(store->carts, int_elem(1), ptr_elem(cart));

    bool added = add_to_cart(store, 1, "Ghost", 1);
    CU_ASSERT_FALSE(added);

    elem_t value;
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(cart->items, string_elem("Ghost"), &value));

    store_destroy(store);
}

// --------------TESTS remove_from_cart--------------
void test_remove_part_of_item()
{
    store_t *store = store_create();

    cart_t *cart = cart_create(1);
    ioopm_hash_table_insert(store->carts, int_elem(1), ptr_elem(cart));
    merch_add_to_store(store, "Apple", "Red apple", 5);

    add_to_cart(store, 1, "Apple", 10);

    bool removed = remove_from_cart(store, 1, "Apple", 4);
    CU_ASSERT_TRUE(removed);

    elem_t value;
    ioopm_hash_table_lookup(cart->items, string_elem("Apple"), &value);
    CU_ASSERT_EQUAL(value.i, 6);

    store_destroy(store);
}

void test_remove_all_of_item()
{
    store_t *store = store_create();

    cart_t *cart = cart_create(1);
    ioopm_hash_table_insert(store->carts, int_elem(1), ptr_elem(cart));
    merch_add_to_store(store, "Apple", "Red apple", 5);

    add_to_cart(store, 1, "Apple", 5);

    bool removed = remove_from_cart(store, 1, "Apple", 5);
    CU_ASSERT_TRUE(removed);

    elem_t value;
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(cart->items, string_elem("Apple"), &value));

    store_destroy(store);
}

void test_remove_more_than_cart_holds()
{
    store_t *store = store_create();

    cart_t *cart = cart_create(1);
    ioopm_hash_table_insert(store->carts, int_elem(1), ptr_elem(cart));
    merch_add_to_store(store, "Apple", "Red apple", 5);

    add_to_cart(store, 1, "Apple", 2);

    bool removed = remove_from_cart(store, 1, "Apple", 3);
    CU_ASSERT_FALSE(removed);

    elem_t value;
    ioopm_hash_table_lookup(cart->items, string_elem("Apple"), &value);
    CU_ASSERT_EQUAL(value.i, 2);

    store_destroy(store);
}

void test_remove_item_not_in_cart()
{
    store_t *store = store_create();

    cart_t *cart = cart_create(1);
    ioopm_hash_table_insert(store->carts, int_elem(1), ptr_elem(cart));
    merch_add_to_store(store, "Apple", "Red apple", 5);

    bool removed = remove_from_cart(store, 1, "Apple", 1);
    CU_ASSERT_FALSE(removed);

    store_destroy(store);
}

// --------------TESTS remove_cart--------------
void test_remove_cart()
{
    store_t *store = store_create();

    cart_t *cart = cart_create(1);
    ioopm_hash_table_insert(store->carts, int_elem(1), ptr_elem(cart));

    bool removed = remove_cart(store, 1);
    CU_ASSERT_TRUE(removed);

    elem_t result;
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(store->carts, int_elem(1), &result));

    store_destroy(store);
}

// --------------TESTS calculate_cost--------------
void test_calculate_cost()
{
    store_t *store = store_create();

    cart_t *cart = cart_create(1);
    ioopm_hash_table_insert(store->carts, int_elem(1), ptr_elem(cart));

    merch_add_to_store(store, "Apple", "Red apple", 5);
    merch_add_to_store(store, "Pear", "Green pear", 10);

    add_to_cart(store, 1, "Apple", 3);
    add_to_cart(store, 1, "Pear", 2);

    int cost = calculate_cost(store, 1);
    CU_ASSERT_EQUAL(cost, 35);

    store_destroy(store);
}

int main() {
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  CU_pSuite my_test_suite = CU_add_suite("Cart test suite", init_suite, clean_suite);
  if (my_test_suite == NULL) {
      CU_cleanup_registry();
      return CU_get_error();
  }

  if ( CU_add_test(my_test_suite, "Store is created with an empty carts table", test_store_create_has_carts) == NULL
       || CU_add_test(my_test_suite, "Create a cart", test_create_cart) == NULL
       || CU_add_test(my_test_suite, "Add a single item to a cart", test_add_single_item) == NULL
       || CU_add_test(my_test_suite, "Add same item twice should sum", test_add_same_item_twice_sums) == NULL
       || CU_add_test(my_test_suite, "Add multiple items to a cart", test_add_mult_items) == NULL
       || CU_add_test(my_test_suite, "Add to nonexisting cart should be invalid", test_add_to_nonexisting_cart) == NULL
       || CU_add_test(my_test_suite, "Add nonexisting merch should be invalid", test_add_nonexisting_merch) == NULL
       || CU_add_test(my_test_suite, "Remove part of an item", test_remove_part_of_item) == NULL
       || CU_add_test(my_test_suite, "Remove all of an item", test_remove_all_of_item) == NULL
       || CU_add_test(my_test_suite, "Remove more than the cart holds should be invalid", test_remove_more_than_cart_holds) == NULL
       || CU_add_test(my_test_suite, "Remove item not in cart should be invalid", test_remove_item_not_in_cart) == NULL
       || CU_add_test(my_test_suite, "Remove a whole cart", test_remove_cart) == NULL
       || CU_add_test(my_test_suite, "Calculate total cost of a cart", test_calculate_cost) == NULL
     )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  CU_basic_set_mode(CU_BRM_NORMAL);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}