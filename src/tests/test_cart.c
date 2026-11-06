
#include <CUnit/Basic.h>
#include "../backend/cart.h"
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

/**
* NOTE: The draft of the tests for this module was created with KLAR (combination of Claude, Chat GTP and Gemini).
* It has been proofread by Signe von Greiff and Anton Äng and changed when needed. 
*/


int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }


// -------------- HELPER FUNCTIONS --------------

static store_t *create_test_store(void)
{
    store_t *store = ioopm_store_create();

    ioopm_merch_add_to_store(store, "Apple", "Red apple", 10);
    ioopm_merch_add_to_store(store, "Banana", "Yellow banana", 5);

    ioopm_replenish_stock(store, "A10", "Apple", 10);
    ioopm_replenish_stock(store, "B20", "Banana", 10);

    return store;
}

static void create_test_cart(store_t *store, size_t id)
{
    cart_t *cart = ioopm_cart_create(id);
    ioopm_add_cart_to_store(store, cart);
}

static merch_t *get_merch(store_t *store, char *name)
{
    elem_t result;

    if (!ioopm_hash_table_lookup(store->merchandise, string_elem(name), &result))
    {
        return NULL;
    }

    return result.p;
}

static shelf_t *get_shelf(store_t *store, char *name)
{
    elem_t result;

    if (!ioopm_hash_table_lookup(store->stock, string_elem(name), &result))
    {
        return NULL;
    }

    return result.p;
}


// -------------- TESTS cart_create --------------

void test_cart_create(void)
{
    cart_t *cart = ioopm_cart_create(1);

    CU_ASSERT_PTR_NOT_NULL(cart);

    if (cart != NULL)
    {
        CU_ASSERT_EQUAL(cart->id, 1);
        CU_ASSERT_PTR_NOT_NULL(cart->items);
        CU_ASSERT_EQUAL(ioopm_hash_table_size(cart->items), 0);

        ioopm_hash_table_destroy(cart->items);
        free(cart);
    }
}

void test_cart_create_different_ids(void)
{
    cart_t *cart1 = ioopm_cart_create(1);
    cart_t *cart2 = ioopm_cart_create(2);

    CU_ASSERT_PTR_NOT_NULL(cart1);
    CU_ASSERT_PTR_NOT_NULL(cart2);

    if (cart1 != NULL && cart2 != NULL)
    {
        CU_ASSERT_NOT_EQUAL(cart1->id, cart2->id);
    }

    if (cart1 != NULL)
    {
        ioopm_hash_table_destroy(cart1->items);
        free(cart1);
    }

    if (cart2 != NULL)
    {
        ioopm_hash_table_destroy(cart2->items);
        free(cart2);
    }
}


// -------------- TESTS add_cart_to_store --------------

void test_add_cart_to_store(void)
{
    store_t *store = ioopm_store_create();

    create_test_cart(store, 1);

    elem_t result;
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(
        store->carts, int_elem(1), &result));

    cart_t *cart = result.p;

    CU_ASSERT_EQUAL(cart->id, 1);

    ioopm_store_destroy(store);
}

void test_add_cart_to_store_increments_next_id(void)
{
    store_t *store = ioopm_store_create();

    CU_ASSERT_EQUAL(store->next_cart_id, 1);

    create_test_cart(store, store->next_cart_id);

    CU_ASSERT_EQUAL(store->next_cart_id, 2);

    create_test_cart(store, store->next_cart_id);

    CU_ASSERT_EQUAL(store->next_cart_id, 3);

    ioopm_store_destroy(store);
}


// -------------- TESTS get_occupied_stock --------------

void test_get_occupied_stock_empty_carts(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    merch_t *apple = get_merch(store, "Apple");

    CU_ASSERT_EQUAL(ioopm_get_occupied_stock(store, apple), 0);

    ioopm_store_destroy(store);
}

void test_get_occupied_stock_one_cart(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 4));

    merch_t *apple = get_merch(store, "Apple");

    CU_ASSERT_EQUAL(ioopm_get_occupied_stock(store, apple), 4);

    ioopm_store_destroy(store);
}

void test_get_occupied_stock_multiple_carts(void)
{
    store_t *store = create_test_store();

    create_test_cart(store, 1);
    create_test_cart(store, 2);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 3));
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 2, "Apple", 4));

    merch_t *apple = get_merch(store, "Apple");

    CU_ASSERT_EQUAL(ioopm_get_occupied_stock(store, apple), 7);

    ioopm_store_destroy(store);
}

void test_get_occupied_stock_ignores_other_merch(void)
{
    store_t *store = create_test_store();

    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 3));
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Banana", 5));

    merch_t *apple = get_merch(store, "Apple");

    CU_ASSERT_EQUAL(ioopm_get_occupied_stock(store, apple), 3);

    ioopm_store_destroy(store);
}


// -------------- TESTS add_to_cart --------------

void test_add_to_cart(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 3));

    elem_t result;
    ioopm_hash_table_lookup(store->carts, int_elem(1), &result);

    cart_t *cart = result.p;

    elem_t quantity;
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(
        cart->items, string_elem("Apple"), &quantity));

    CU_ASSERT_EQUAL(quantity.i, 3);

   ioopm_store_destroy(store);
}

void test_add_same_item_twice(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 2));
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 3));

    elem_t result;
    ioopm_hash_table_lookup(store->carts, int_elem(1), &result);

    cart_t *cart = result.p;

    elem_t quantity;
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(
        cart->items, string_elem("Apple"), &quantity));

    CU_ASSERT_EQUAL(quantity.i, 5);

    ioopm_store_destroy(store);
}

void test_add_different_items_to_cart(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 2));
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Banana", 4));

    elem_t result;
    ioopm_hash_table_lookup(store->carts, int_elem(1), &result);

    cart_t *cart = result.p;

    elem_t quantity;

    CU_ASSERT_TRUE(ioopm_hash_table_lookup(
        cart->items, string_elem("Apple"), &quantity));
    CU_ASSERT_EQUAL(quantity.i, 2);

    CU_ASSERT_TRUE(ioopm_hash_table_lookup(
        cart->items, string_elem("Banana"), &quantity));
    CU_ASSERT_EQUAL(quantity.i, 4);

    CU_ASSERT_EQUAL(ioopm_hash_table_size(cart->items), 2);

    ioopm_store_destroy(store);
}

void test_add_to_nonexistent_cart(void)
{
    store_t *store = create_test_store();

    CU_ASSERT_FALSE(ioopm_add_to_cart(store, 99, "Apple", 1));

    ioopm_store_destroy(store);
}

void test_add_nonexistent_merchandise(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_FALSE(ioopm_add_to_cart(store, 1, "Pineapple", 1));

    ioopm_store_destroy(store);
}

void test_add_more_than_total_stock(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_FALSE(ioopm_add_to_cart(store, 1, "Apple", 11));

    ioopm_store_destroy(store);
}

void test_add_zero_quantity(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    // Expected behaviour: zero quantity should not be added.
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 0));

    ioopm_store_destroy(store);
}


// -------------- TESTS stock reservation across carts --------------

void test_reserve_stock_across_multiple_carts(void)
{
    store_t *store = create_test_store();

    create_test_cart(store, 1);
    create_test_cart(store, 2);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 6));

    // Only four apples remain available.
    CU_ASSERT_FALSE(ioopm_add_to_cart(store, 2, "Apple", 5));
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 2, "Apple", 4));

    merch_t *apple = get_merch(store, "Apple");

    CU_ASSERT_EQUAL(ioopm_get_occupied_stock(store, apple), 10);

    ioopm_store_destroy(store);
}

void test_reserve_stock_same_cart_multiple_times(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 4));
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 6));

    CU_ASSERT_FALSE(ioopm_add_to_cart(store, 1, "Apple", 1));

    merch_t *apple = get_merch(store, "Apple");

    CU_ASSERT_EQUAL(ioopm_get_occupied_stock(store, apple), 10);

    ioopm_store_destroy(store);
}

void test_reserve_stock_different_merchandise(void)
{
    store_t *store = create_test_store();

    create_test_cart(store, 1);
    create_test_cart(store, 2);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 10));
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 2, "Banana", 10));

    merch_t *apple = get_merch(store, "Apple");
    merch_t *banana = get_merch(store, "Banana");

    CU_ASSERT_EQUAL(ioopm_get_occupied_stock(store, apple), 10);
    CU_ASSERT_EQUAL(ioopm_get_occupied_stock(store, banana), 10);

    ioopm_store_destroy(store);
}


// -------------- TESTS remove_cart --------------

void test_remove_cart(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_remove_cart(store, 1));

    CU_ASSERT_FALSE(ioopm_hash_table_has_key(
        store->carts, int_elem(1)));

    ioopm_store_destroy(store);
}

void test_remove_nonexistent_cart(void)
{
    store_t *store = create_test_store();

    CU_ASSERT_FALSE(ioopm_remove_cart(store, 99));

    ioopm_store_destroy(store);
}

void test_remove_cart_releases_stock_reservation(void)
{
    store_t *store = create_test_store();

    create_test_cart(store, 1);
    create_test_cart(store, 2);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 6));
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 2, "Apple", 4));

    CU_ASSERT_TRUE(ioopm_remove_cart(store, 1));

    // The six apples reserved by cart 1 are now available.
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 2, "Apple", 6));

    merch_t *apple = get_merch(store, "Apple");

    CU_ASSERT_EQUAL(ioopm_get_occupied_stock(store, apple), 10);

    ioopm_store_destroy(store);
}


// -------------- TESTS remove_from_cart --------------

void test_remove_from_cart_partial_quantity(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 5));
    CU_ASSERT_TRUE(ioopm_remove_from_cart(store, 1, "Apple", 2));

    elem_t result;
    ioopm_hash_table_lookup(store->carts, int_elem(1), &result);

    cart_t *cart = result.p;

    elem_t quantity;
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(
        cart->items, string_elem("Apple"), &quantity));

    CU_ASSERT_EQUAL(quantity.i, 3);

    ioopm_store_destroy(store);
}

void test_remove_from_cart_all_quantity(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 5));
    CU_ASSERT_TRUE(ioopm_remove_from_cart(store, 1, "Apple", 5));

    elem_t result;
    ioopm_hash_table_lookup(store->carts, int_elem(1), &result);

    cart_t *cart = result.p;

    CU_ASSERT_FALSE(ioopm_hash_table_has_key(
        cart->items, string_elem("Apple")));

    ioopm_store_destroy(store);
}

void test_remove_more_than_cart_contains(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 3));

    CU_ASSERT_FALSE(ioopm_remove_from_cart(store, 1, "Apple", 4));

    ioopm_store_destroy(store);
}

void test_remove_from_nonexistent_cart(void)
{
    store_t *store = create_test_store();

    CU_ASSERT_FALSE(ioopm_remove_from_cart(store, 99, "Apple", 1));

    ioopm_store_destroy(store);
}

void test_remove_nonexistent_item_from_cart(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_FALSE(ioopm_remove_from_cart(store, 1, "Apple", 1));

    ioopm_store_destroy(store);
}

void test_remove_zero_quantity(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 5));

    // Removing zero items should leave the quantity unchanged.
    CU_ASSERT_TRUE(ioopm_remove_from_cart(store, 1, "Apple", 0));

    elem_t result;
    ioopm_hash_table_lookup(store->carts, int_elem(1), &result);

    cart_t *cart = result.p;

    elem_t quantity;
    ioopm_hash_table_lookup(
        cart->items, string_elem("Apple"), &quantity);

    CU_ASSERT_EQUAL(quantity.i, 5);

    ioopm_store_destroy(store);
}


// -------------- TESTS calculate_cost --------------

void test_calculate_cost(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 2));
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Banana", 3));

    // 2 * 10 + 3 * 5 = 35
    CU_ASSERT_EQUAL(ioopm_calculate_cost(store, 1), 35);

    ioopm_store_destroy(store);
}

void test_calculate_cost_empty_cart(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_EQUAL(ioopm_calculate_cost(store, 1), 0);

    ioopm_store_destroy(store);
}

void test_calculate_cost_nonexistent_cart(void)
{
    store_t *store = create_test_store();

    CU_ASSERT_EQUAL(ioopm_calculate_cost(store, 99), 0);

    ioopm_store_destroy(store);
}

void test_calculate_cost_after_removing_items(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 5));
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Banana", 2));

    CU_ASSERT_TRUE(ioopm_remove_from_cart(store, 1, "Apple", 3));

    // 2 * 10 + 2 * 5 = 30
    CU_ASSERT_EQUAL(ioopm_calculate_cost(store, 1), 30);

    ioopm_store_destroy(store);
}


// -------------- TESTS check_if_all_items_valid --------------

void test_check_if_all_items_valid(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 5));

    elem_t result;
    ioopm_hash_table_lookup(store->carts, int_elem(1), &result);

    cart_t *cart = result.p;

    CU_ASSERT_TRUE(ioopm_check_if_all_items_valid(store, cart->items));

    ioopm_store_destroy(store);
}

void test_check_if_all_items_valid_empty_cart(void)
{
    store_t *store = create_test_store();

    ioopm_hash_table_t *items = ioopm_hash_table_create(
        ioopm_string_knr_hash, string_compare, NULL);

    CU_ASSERT_TRUE(ioopm_check_if_all_items_valid(store, items));

    ioopm_hash_table_destroy(items);
    ioopm_store_destroy(store);
}

void test_check_if_all_items_valid_null_store(void)
{
    ioopm_hash_table_t *items = ioopm_hash_table_create(
        ioopm_string_knr_hash, string_compare, NULL);

    CU_ASSERT_FALSE(ioopm_check_if_all_items_valid(NULL, items));

    ioopm_hash_table_destroy(items);
}

void test_check_if_all_items_valid_null_items(void)
{
    store_t *store = create_test_store();

    CU_ASSERT_FALSE(ioopm_check_if_all_items_valid(store, NULL));

    ioopm_store_destroy(store);
}

void test_check_if_all_items_valid_rejects_unknown_merch(void)
{
    store_t *store = create_test_store();

    ioopm_hash_table_t *items = ioopm_hash_table_create(
        ioopm_string_knr_hash, string_compare, NULL);

    char *unknown_name = strdup("Unknown item");

    ioopm_hash_table_insert(
        items, string_elem(unknown_name), int_elem(1));

    CU_ASSERT_FALSE(ioopm_check_if_all_items_valid(store, items));

    ioopm_hash_table_destroy(items);
    free(unknown_name);

    ioopm_store_destroy(store);
}

void test_check_if_all_items_valid_rejects_excess_quantity(void)
{
    store_t *store = create_test_store();

    ioopm_hash_table_t *items = ioopm_hash_table_create(
        ioopm_string_knr_hash, string_compare, NULL);

    char *apple_name = strdup("Apple");

    ioopm_hash_table_insert(
        items, string_elem(apple_name), int_elem(11));

    CU_ASSERT_FALSE(ioopm_check_if_all_items_valid(store, items));

    ioopm_hash_table_destroy(items);
    free(apple_name);

    ioopm_store_destroy(store);
}


// -------------- TESTS checkout --------------

void test_checkout_decreases_stock(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 4));
    CU_ASSERT_TRUE(ioopm_checkout(store, 1));

    merch_t *apple = get_merch(store, "Apple");

    CU_ASSERT_EQUAL(ioopm_get_total_stock(apple), 6);

    CU_ASSERT_FALSE(ioopm_hash_table_has_key(
        store->carts, int_elem(1)));

    ioopm_store_destroy(store);
}

void test_checkout_removes_empty_shelf(void)
{
    store_t *store = ioopm_store_create();

    ioopm_merch_add_to_store(store, "Apple", "Red apple", 10);
    ioopm_replenish_stock(store, "A10", "Apple", 3);
    ioopm_replenish_stock(store, "B20", "Apple", 4);

    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 3));
    CU_ASSERT_TRUE(ioopm_checkout(store, 1));

    merch_t *apple = get_merch(store, "Apple");

    CU_ASSERT_EQUAL(ioopm_get_total_stock(apple), 4);
    CU_ASSERT_PTR_NULL(get_shelf(store, "A10"));

    shelf_t *shelf = get_shelf(store, "B20");

    CU_ASSERT_PTR_NOT_NULL(shelf);

    if (shelf != NULL)
    {
        CU_ASSERT_EQUAL(shelf->quantity, 4);
    }

    ioopm_store_destroy(store);
}

void test_checkout_across_multiple_shelves(void)
{
    store_t *store = ioopm_store_create();

    ioopm_merch_add_to_store(store, "Apple", "Red apple", 10);
    ioopm_replenish_stock(store, "A10", "Apple", 3);
    ioopm_replenish_stock(store, "B20", "Apple", 4);

    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 5));
    CU_ASSERT_TRUE(ioopm_checkout(store, 1));

    merch_t *apple = get_merch(store, "Apple");

    // Five apples consumed from seven total.
    CU_ASSERT_EQUAL(ioopm_get_total_stock(apple), 2);

    // First shelf should be removed.
    CU_ASSERT_PTR_NULL(get_shelf(store, "A10"));

    // Two apples should remain on the second shelf.
    shelf_t *shelf = get_shelf(store, "B20");

    CU_ASSERT_PTR_NOT_NULL(shelf);

    if (shelf != NULL)
    {
        CU_ASSERT_EQUAL(shelf->quantity, 2);
    }

    ioopm_store_destroy(store);
}

void test_checkout_exact_shelf_quantity(void)
{
    store_t *store = ioopm_store_create();

    ioopm_merch_add_to_store(store, "Apple", "Red apple", 10);
    ioopm_replenish_stock(store, "A10", "Apple", 3);
    ioopm_replenish_stock(store, "B20", "Apple", 4);

    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 3));
    CU_ASSERT_TRUE(ioopm_checkout(store, 1));

    merch_t *apple = get_merch(store, "Apple");

    CU_ASSERT_EQUAL(ioopm_get_total_stock(apple), 4);
    CU_ASSERT_PTR_NULL(get_shelf(store, "A10"));

    shelf_t *shelf = get_shelf(store, "B20");

    CU_ASSERT_PTR_NOT_NULL(shelf);

    if (shelf != NULL)
    {
        CU_ASSERT_EQUAL(shelf->quantity, 4);
    }

    ioopm_store_destroy(store);
}

void test_checkout_multiple_items(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Apple", 2));
    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 1, "Banana", 3));

    CU_ASSERT_TRUE(ioopm_checkout(store, 1));

    CU_ASSERT_EQUAL(ioopm_get_total_stock(get_merch(store, "Apple")), 8);
    CU_ASSERT_EQUAL(ioopm_get_total_stock(get_merch(store, "Banana")), 7);

    CU_ASSERT_FALSE(ioopm_hash_table_has_key(
        store->carts, int_elem(1)));

    ioopm_store_destroy(store);
}

void test_checkout_empty_cart(void)
{
    store_t *store = create_test_store();
    create_test_cart(store, 1);

    CU_ASSERT_TRUE(ioopm_checkout(store, 1));

    CU_ASSERT_FALSE(ioopm_hash_table_has_key(
        store->carts, int_elem(1)));

    CU_ASSERT_EQUAL(ioopm_get_total_stock(get_merch(store, "Apple")), 10);

    ioopm_store_destroy(store);
}

void test_checkout_nonexistent_cart(void)
{
    store_t *store = create_test_store();

    CU_ASSERT_FALSE(ioopm_checkout(store, 99));

    ioopm_store_destroy(store);
}


// -------------- TESTS main --------------

int main()
{
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    CU_pSuite my_test_suite = CU_add_suite(
        "Cart test suite", init_suite, clean_suite);

    if (my_test_suite == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(my_test_suite, "Create cart", test_cart_create) == NULL
        || CU_add_test(my_test_suite, "Create carts with different IDs", test_cart_create_different_ids) == NULL
        || CU_add_test(my_test_suite, "Add cart to store", test_add_cart_to_store) == NULL
        || CU_add_test(my_test_suite, "Add cart increments next ID", test_add_cart_to_store_increments_next_id) == NULL

        || CU_add_test(my_test_suite, "Occupied stock: empty carts", test_get_occupied_stock_empty_carts) == NULL
        || CU_add_test(my_test_suite, "Occupied stock: one cart", test_get_occupied_stock_one_cart) == NULL
        || CU_add_test(my_test_suite, "Occupied stock: multiple carts", test_get_occupied_stock_multiple_carts) == NULL
        || CU_add_test(my_test_suite, "Occupied stock: ignores other merchandise", test_get_occupied_stock_ignores_other_merch) == NULL

        || CU_add_test(my_test_suite, "Add to cart", test_add_to_cart) == NULL
        || CU_add_test(my_test_suite, "Add same item twice", test_add_same_item_twice) == NULL
        || CU_add_test(my_test_suite, "Add different items", test_add_different_items_to_cart) == NULL
        || CU_add_test(my_test_suite, "Add to nonexistent cart", test_add_to_nonexistent_cart) == NULL
        || CU_add_test(my_test_suite, "Add nonexistent merchandise", test_add_nonexistent_merchandise) == NULL
        || CU_add_test(my_test_suite, "Add more than total stock", test_add_more_than_total_stock) == NULL
        || CU_add_test(my_test_suite, "Add zero quantity", test_add_zero_quantity) == NULL

        || CU_add_test(my_test_suite, "Reserve stock across carts", test_reserve_stock_across_multiple_carts) == NULL
        || CU_add_test(my_test_suite, "Reserve stock repeatedly in same cart", test_reserve_stock_same_cart_multiple_times) == NULL
        || CU_add_test(my_test_suite, "Reserve different merchandise", test_reserve_stock_different_merchandise) == NULL

        || CU_add_test(my_test_suite, "Remove cart", test_remove_cart) == NULL
        || CU_add_test(my_test_suite, "Remove nonexistent cart", test_remove_nonexistent_cart) == NULL
        || CU_add_test(my_test_suite, "Remove cart releases reservation", test_remove_cart_releases_stock_reservation) == NULL

        || CU_add_test(my_test_suite, "Remove partial item quantity", test_remove_from_cart_partial_quantity) == NULL
        || CU_add_test(my_test_suite, "Remove all item quantity", test_remove_from_cart_all_quantity) == NULL
        || CU_add_test(my_test_suite, "Remove more than cart contains", test_remove_more_than_cart_contains) == NULL
        || CU_add_test(my_test_suite, "Remove from nonexistent cart", test_remove_from_nonexistent_cart) == NULL
        || CU_add_test(my_test_suite, "Remove nonexistent item", test_remove_nonexistent_item_from_cart) == NULL
        || CU_add_test(my_test_suite, "Remove zero quantity", test_remove_zero_quantity) == NULL

        || CU_add_test(my_test_suite, "Calculate cost", test_calculate_cost) == NULL
        || CU_add_test(my_test_suite, "Calculate cost of empty cart", test_calculate_cost_empty_cart) == NULL
        || CU_add_test(my_test_suite, "Calculate cost of nonexistent cart", test_calculate_cost_nonexistent_cart) == NULL
        || CU_add_test(my_test_suite, "Calculate cost after removing items", test_calculate_cost_after_removing_items) == NULL

        || CU_add_test(my_test_suite, "Validate cart items", test_check_if_all_items_valid) == NULL
        || CU_add_test(my_test_suite, "Validate empty cart", test_check_if_all_items_valid_empty_cart) == NULL
        || CU_add_test(my_test_suite, "Validate null store", test_check_if_all_items_valid_null_store) == NULL
        || CU_add_test(my_test_suite, "Validate null items", test_check_if_all_items_valid_null_items) == NULL
        || CU_add_test(my_test_suite, "Reject unknown merchandise", test_check_if_all_items_valid_rejects_unknown_merch) == NULL
        || CU_add_test(my_test_suite, "Reject excessive quantity", test_check_if_all_items_valid_rejects_excess_quantity) == NULL

        || CU_add_test(my_test_suite, "Checkout decreases stock", test_checkout_decreases_stock) == NULL
        || CU_add_test(my_test_suite, "Checkout removes empty shelf", test_checkout_removes_empty_shelf) == NULL
        || CU_add_test(my_test_suite, "Checkout across multiple shelves", test_checkout_across_multiple_shelves) == NULL
        || CU_add_test(my_test_suite, "Checkout exact shelf quantity", test_checkout_exact_shelf_quantity) == NULL
        || CU_add_test(my_test_suite, "Checkout multiple items", test_checkout_multiple_items) == NULL
        || CU_add_test(my_test_suite, "Checkout empty cart", test_checkout_empty_cart) == NULL
        || CU_add_test(my_test_suite, "Checkout nonexistent cart", test_checkout_nonexistent_cart) == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_NORMAL);
    CU_basic_run_tests();

    CU_cleanup_registry();

    return CU_get_error();
}