#include <CUnit/Basic.h>
#include "../backend/store.h"
#include "../backend/merch.h"
#include "../backend/stock.h"
#include "../backend/cart.h"
#include "../data_structures/hash_table.h"
#include "../utils/utils.h"
#include <stdbool.h>
#include <stdlib.h>

int init_suite(void)
{
    return 0;
}

int clean_suite(void)
{
    return 0;
}

// store_create: all three tables exist and are empty
void test_store_create_tables(void)
{
    store_t *store = store_create();

    CU_ASSERT_PTR_NOT_NULL(store);
    CU_ASSERT_PTR_NOT_NULL(store->merchandise);
    CU_ASSERT_PTR_NOT_NULL(store->stock);
    CU_ASSERT_PTR_NOT_NULL(store->carts);

    CU_ASSERT_TRUE(ioopm_hash_table_is_empty(store->merchandise));
    CU_ASSERT_TRUE(ioopm_hash_table_is_empty(store->stock));
    CU_ASSERT_TRUE(ioopm_hash_table_is_empty(store->carts));

    store_destroy(store);
}

// store_stock and store_merchandise hand back the store's own tables
void test_store_getters_return_the_tables(void)
{
    store_t *store = store_create();

    CU_ASSERT_PTR_EQUAL(store_stock(store), store->stock);
    CU_ASSERT_PTR_EQUAL(store_merchandise(store), store->merchandise);

    store_destroy(store);
}

// Adding merch and stock is visible through the getters
void test_store_getters_see_inserted_data(void)
{
    store_t *store = store_create();

    merch_add_to_store(store, "Apple", "Red apple", 5);
    replenish_stock(store, "A10", "Apple", 3);

    elem_t value;

    bool has_apple = ioopm_hash_table_lookup(store_merchandise(store), string_elem("Apple"), &value);
    bool has_shelf = ioopm_hash_table_lookup(store_stock(store), string_elem("A10"), &value);

    CU_ASSERT_TRUE(has_apple);
    CU_ASSERT_TRUE(has_shelf);

    store_destroy(store);
}

// merchandise and stock must stay in sync after an add
void test_store_merch_and_stock_agree(void)
{
    store_t *store = store_create();

    merch_add_to_store(store, "Apple", "Red apple", 5);
    replenish_stock(store, "A10", "Apple", 3);

    bool in_merch = ioopm_hash_table_has_key(store->merchandise, string_elem("Apple"));
    bool in_stock = ioopm_hash_table_has_key(store->stock, string_elem("A10"));

    CU_ASSERT_TRUE(in_merch);
    CU_ASSERT_TRUE(in_stock);

    store_destroy(store);
}

// Removing a merch must clear both tables
void test_store_remove_merch_clears_both_tables(void)
{
    store_t *store = store_create();

    merch_add_to_store(store, "Apple", "Red apple", 5);
    replenish_stock(store, "A10", "Apple", 3);

    merch_remove_from_store(store, "Apple");

    bool in_merch = ioopm_hash_table_has_key(store->merchandise, string_elem("Apple"));
    bool in_stock = ioopm_hash_table_has_key(store->stock, string_elem("A10"));

    CU_ASSERT_FALSE(in_merch);
    CU_ASSERT_FALSE(in_stock);

    store_destroy(store);
}

// A cart lives in the carts table and is gone once removed
void test_store_cart_add_and_remove(void)
{
    store_t *store = store_create();

    cart_t *cart = cart_create(1);
    ioopm_hash_table_insert(store->carts, int_elem(1), ptr_elem(cart));

    CU_ASSERT_TRUE(ioopm_hash_table_has_key(store->carts, int_elem(1)));

    remove_cart(store, 1);

    CU_ASSERT_FALSE(ioopm_hash_table_has_key(store->carts, int_elem(1)));

    store_destroy(store);
}

// store_destroy must free everything inside the tables.
// The assertions are only the tables being empty; the real check is valgrind.
void test_store_destroy_frees_contents(void)
{
    store_t *store = store_create();

    merch_add_to_store(store, "Apple", "Red apple", 5);
    merch_add_to_store(store, "Pear", "Green pear", 10);
    replenish_stock(store, "A10", "Apple", 3);
    replenish_stock(store, "B01", "Pear", 7);

    cart_t *cart = cart_create(1);
    ioopm_hash_table_insert(store->carts, int_elem(1), ptr_elem(cart));
    add_to_cart(store, 1, "Apple", 2);
    add_to_cart(store, 1, "Pear", 1);

    CU_ASSERT_EQUAL(ioopm_hash_table_size(store->merchandise), 2);
    CU_ASSERT_EQUAL(ioopm_hash_table_size(store->stock), 2);
    CU_ASSERT_EQUAL(ioopm_hash_table_size(store->carts), 1);

    store_destroy(store);
}

// Two stores must not share any table
void test_two_stores_are_independent(void)
{
    store_t *store_a = store_create();
    store_t *store_b = store_create();

    CU_ASSERT_PTR_NOT_EQUAL(store_a->merchandise, store_b->merchandise);
    CU_ASSERT_PTR_NOT_EQUAL(store_a->stock, store_b->stock);
    CU_ASSERT_PTR_NOT_EQUAL(store_a->carts, store_b->carts);

    merch_add_to_store(store_a, "Apple", "Red apple", 5);

    CU_ASSERT_TRUE(ioopm_hash_table_has_key(store_a->merchandise, string_elem("Apple")));
    CU_ASSERT_FALSE(ioopm_hash_table_has_key(store_b->merchandise, string_elem("Apple")));

    store_destroy(store_a);
    store_destroy(store_b);
}

int main(void)
{
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    CU_pSuite suite = CU_add_suite("Store test suite", init_suite, clean_suite);
    if (suite == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(suite, "Store is created with three empty tables", test_store_create_tables) == NULL
        || CU_add_test(suite, "Getters return the store's own tables", test_store_getters_return_the_tables) == NULL
        || CU_add_test(suite, "Getters see inserted data", test_store_getters_see_inserted_data) == NULL
        || CU_add_test(suite, "Merch and stock stay in sync", test_store_merch_and_stock_agree) == NULL
        || CU_add_test(suite, "Removing merch clears both tables", test_store_remove_merch_clears_both_tables) == NULL
        || CU_add_test(suite, "Cart can be added and removed", test_store_cart_add_and_remove) == NULL
        || CU_add_test(suite, "Destroy frees everything inside", test_store_destroy_frees_contents) == NULL
        || CU_add_test(suite, "Two stores are independent", test_two_stores_are_independent) == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_NORMAL);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}