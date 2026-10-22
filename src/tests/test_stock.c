#include <CUnit/Basic.h>
#include "../backend/merch.h"
#include "../backend/store.h"
#include "../backend/stock.h"
#include "../data_structures/hash_table.h"
#include "../data_structures/hash_table_iterator.h"
#include "../data_structures/linked_list.h"
#include "../data_structures/list_iterator.h"
#include "../utils/utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

// -------------- TESTS shelf_create --------------
void test_shelf_create(void)
{
    shelf_t *shelf = shelf_create("A10", "Apple");

    CU_ASSERT_PTR_NOT_NULL(shelf);
    CU_ASSERT_PTR_NOT_NULL(shelf->shelf_name);
    CU_ASSERT_PTR_NOT_NULL(shelf->merch_name);
    CU_ASSERT_STRING_EQUAL(shelf->shelf_name, "A10");
    CU_ASSERT_STRING_EQUAL(shelf->merch_name, "Apple");

    free(shelf->shelf_name);
    free(shelf->merch_name);
    free(shelf);
}

void test_shelf_create_copies_strings(void)
{
    char label[] = "A10";
    shelf_t *shelf = shelf_create(label, "Apple");
    
    // Mutate the caller's buffer; the shelf must be unaffected
    label[0] = 'X';
    CU_ASSERT_STRING_EQUAL(shelf->shelf_name, "A10");
    
    free(shelf->shelf_name);
    free(shelf->merch_name);
    free(shelf);
}

// -------------- TESTS comparison_shelf --------------
void test_comparison_shelf_equal(void)
{
    elem_t a = string_elem("A10");
    elem_t b = string_elem("A10");
    CU_ASSERT_EQUAL(comparison_shelf(a, b), 0);
}

void test_comparison_shelf_order(void)
{
    elem_t a = string_elem("A10");
    elem_t b = string_elem("B12");
    
    CU_ASSERT_TRUE(comparison_shelf(a, b) < 0);
    CU_ASSERT_TRUE(comparison_shelf(b, a) > 0);
}

// -------------- TESTS replenish_stock --------------
void test_replenish_new_shelf(void)
{
    store_t *store = store_create();
    CU_ASSERT_TRUE(merch_add_to_store(store, "Apple", "Red apple", 10));
    
    CU_ASSERT_TRUE(replenish_stock(store, "A10", "Apple", 7));
    
    elem_t result;
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(store->stock, string_elem("A10"), &result));
    
    shelf_t *shelf = result.p;
    CU_ASSERT_STRING_EQUAL(shelf->shelf_name, "A10");
    CU_ASSERT_STRING_EQUAL(shelf->merch_name, "Apple");
    CU_ASSERT_EQUAL(shelf->quantity, 7);
    
    store_destroy(store);
}

void test_replenish_aliases_into_merch_list(void)
{
    store_t *store = store_create();
    merch_add_to_store(store, "Apple", "Red apple", 10);
    replenish_stock(store, "A10", "Apple", 7);
    
    // The empty file is built up by the course backend. Verify the test
    elem_t merch_result;
    ioopm_hash_table_lookup(store->merchandise, string_elem("Apple"), &merch_result);
    merch_t *merch = merch_result.p;
    
    CU_ASSERT_PTR_NOT_NULL(merch->shelfs);
    CU_ASSERT_EQUAL(ioopm_list_size(merch->shelfs), 1);
    
    // The list and the stock table must point at the SAME shelf
    elem_t stock_result;
    ioopm_hash_table_lookup(store->stock, string_elem("A10"), &stock_result);
    shelf_t *from_stock = stock_result.p;
    shelf_t *from_list = merch->shelfs->first->head.p;
    
    CU_ASSERT_PTR_EQUAL(from_stock, from_list);
    
    store_destroy(store);
}

void test_replenish_existing_shelf_adds_quantity(void)
{
    store_t *store = store_create();
    merch_add_to_store(store, "Apple", "Red apple", 10);
    
    CU_ASSERT_TRUE(replenish_stock(store, "A10", "Apple", 7));
    CU_ASSERT_TRUE(replenish_stock(store, "A10", "Apple", 5));
    
    elem_t result;
    ioopm_hash_table_lookup(store->stock, string_elem("A10"), &result);
    shelf_t *shelf = result.p;
    
    CU_ASSERT_EQUAL(shelf->quantity, 12);
    
    // Still only one shelf in the merch list
    elem_t merch_result;
    ioopm_hash_table_lookup(store->merchandise, string_elem("Apple"), &merch_result);
    merch_t *merch = merch_result.p;
    CU_ASSERT_EQUAL(ioopm_list_size(merch->shelfs), 1);
    
    store_destroy(store);
}

void test_replenish_nonexisting_merch(void)
{
    store_t *store = store_create();
    
    CU_ASSERT_FALSE(replenish_stock(store, "A10", "Apple", 7));
    
    // Nothing may be inserted into stock
    elem_t result;
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(store->stock, string_elem("A10"), &result));
    
    store_destroy(store);
}

void test_replenish_shelf_occupied_by_other_merch(void)
{
    store_t *store = store_create();
    merch_add_to_store(store, "Apple", "Red apple", 10);
    merch_add_to_store(store, "Pear", "Green pear", 15);
    
    CU_ASSERT_TRUE(replenish_stock(store, "A10", "Apple", 7));
    
    // A10 already holds Apple, so it cannot hold Pear
    CU_ASSERT_FALSE(replenish_stock(store, "A10", "Pear", 3));
    
    elem_t result;
    ioopm_hash_table_lookup(store->stock, string_elem("A10"), &result);
    shelf_t *shelf = result.p;
    
    CU_ASSERT_STRING_EQUAL(shelf->merch_name, "Apple");
    CU_ASSERT_EQUAL(shelf->quantity, 7);
    
    store_destroy(store);
}

void test_replenish_zero_quantity(void)
{
    store_t *store = store_create();
    merch_add_to_store(store, "Apple", "Red apple", 10);
    
    CU_ASSERT_FALSE(replenish_stock(store, "A10", "Apple", 0));
    
    elem_t result;
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(store->stock, string_elem("A10"), &result));
    
    store_destroy(store);
}

// -------------- TESTS list_shelves --------------
void test_list_shelves_sorted(void)
{
    store_t *store = store_create();
    merch_add_to_store(store, "Apple", "Red apple", 10);
    
    replenish_stock(store, "C30", "Apple", 1);
    replenish_stock(store, "A10", "Apple", 1);
    replenish_stock(store, "B20", "Apple", 1);
    
    elem_t merch_result;
    ioopm_hash_table_lookup(store->merchandise, string_elem("Apple"), &merch_result);
    merch_t *merch = merch_result.p;
    
    size_t size = 0;
    elem_t *shelves = list_shelves(merch, &size);
    
    CU_ASSERT_EQUAL(size, 3);
    
    CU_ASSERT_STRING_EQUAL(shelves[0].s, "A10");
    CU_ASSERT_STRING_EQUAL(shelves[1].s, "B20");
    CU_ASSERT_STRING_EQUAL(shelves[2].s, "C30");
    
    free(shelves);
    store_destroy(store);
}

void test_list_shelves_single(void)
{
    store_t *store = store_create();
    merch_add_to_store(store, "Apple", "Red apple", 10);
    replenish_stock(store, "A10", "Apple", 4);

    elem_t merch_result;
    ioopm_hash_table_lookup(store->merchandise, string_elem("Apple"), &merch_result);
    merch_t *merch = merch_result.p;

    size_t size = 0;
    elem_t *shelves = list_shelves(merch, &size);

    CU_ASSERT_EQUAL(size, 1);
    CU_ASSERT_STRING_EQUAL(shelves[0].s, "A10");

    free(shelves);
    store_destroy(store);
}

// -------------- TESTS stock_remove_merch --------------
void test_stock_remove_merch(void)
{
    store_t *store = store_create();
    merch_add_to_store(store, "Apple", "Red apple", 10);

    replenish_stock(store, "A10", "Apple", 3);
    replenish_stock(store, "B20", "Apple", 5);

    elem_t merch_result;
    ioopm_hash_table_lookup(store->merchandise, string_elem("Apple"), &merch_result);
    merch_t *merch = merch_result.p;

    CU_ASSERT_TRUE(stock_remove_merch(store, merch));

    elem_t result;
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(store->stock, string_elem("A10"), &result));
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(store->stock, string_elem("B20"), &result));

    store_destroy(store);
}

void test_stock_remove_merch_leaves_other_merch(void)
{
    store_t *store = store_create();
    merch_add_to_store(store, "Apple", "Red apple", 10);
    merch_add_to_store(store, "Pear", "Green pear", 15);

    replenish_stock(store, "A10", "Apple", 3);
    replenish_stock(store, "B20", "Pear", 4);

    elem_t merch_result;
    ioopm_hash_table_lookup(store->merchandise, string_elem("Apple"), &merch_result);
    merch_t *apple = merch_result.p;

    CU_ASSERT_TRUE(stock_remove_merch(store, apple));

    elem_t result;
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(store->stock, string_elem("A10"), &result));
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(store->stock, string_elem("B20"), &result));

    store_destroy(store);
}

// -------------- TESTS stock_update_merch_name --------------
void test_stock_update_merch_name(void)
{
    store_t *store = store_create();
    merch_add_to_store(store, "Apple", "Red apple", 10);

    replenish_stock(store, "A10", "Apple", 3);
    replenish_stock(store, "B20", "Apple", 5);

    elem_t merch_result;
    ioopm_hash_table_lookup(store->merchandise, string_elem("Apple"), &merch_result);

    // Rename through the public edit function
    CU_ASSERT_TRUE(merch_edit(store, "Apple", "GreenApple", "Green apple", 12));

    elem_t shelf_result;
    ioopm_hash_table_lookup(store->stock, string_elem("A10"), &shelf_result);
    CU_ASSERT_STRING_EQUAL(((shelf_t *) shelf_result.p)->merch_name, "GreenApple");

    ioopm_hash_table_lookup(store->stock, string_elem("B20"), &shelf_result);
    CU_ASSERT_STRING_EQUAL(((shelf_t *) shelf_result.p)->merch_name, "GreenApple");

    store_destroy(store);
}

int main()
{
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    CU_pSuite my_test_suite = CU_add_suite("Stock test suite", init_suite, clean_suite);
    if (my_test_suite == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(my_test_suite, "Shelf creation", test_shelf_create) == NULL
        || CU_add_test(my_test_suite, "Shelf creation copies strings", test_shelf_create_copies_strings) == NULL
        || CU_add_test(my_test_suite, "Comparison: equal", test_comparison_shelf_equal) == NULL
        || CU_add_test(my_test_suite, "Comparison: order", test_comparison_shelf_order) == NULL
        || CU_add_test(my_test_suite, "Replenish creates new shelf", test_replenish_new_shelf) == NULL
        || CU_add_test(my_test_suite, "Replenish aliases into merch list", test_replenish_aliases_into_merch_list) == NULL
        || CU_add_test(my_test_suite, "Replenish adds to existing shelf", test_replenish_existing_shelf_adds_quantity) == NULL
        || CU_add_test(my_test_suite, "Replenish nonexisting merch is invalid", test_replenish_nonexisting_merch) == NULL
        || CU_add_test(my_test_suite, "Replenish shelf occupied by other merch", test_replenish_shelf_occupied_by_other_merch) == NULL
        || CU_add_test(my_test_suite, "Replenish zero quantity is invalid", test_replenish_zero_quantity) == NULL
        || CU_add_test(my_test_suite, "List shelves: sorted", test_list_shelves_sorted) == NULL
        || CU_add_test(my_test_suite, "List shelves: single", test_list_shelves_single) == NULL
        || CU_add_test(my_test_suite, "Remove all shelves of a merch", test_stock_remove_merch) == NULL
        || CU_add_test(my_test_suite, "Remove shelves leaves other merch", test_stock_remove_merch_leaves_other_merch) == NULL
        || CU_add_test(my_test_suite, "Update merch name on shelves", test_stock_update_merch_name) == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_NORMAL);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}