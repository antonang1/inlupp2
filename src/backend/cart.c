#include "merch.h"
#include "cart.h"
#include "store.c"
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>
#include "../data_structures/hash_table.h"
#include "../data_structures/hash_table_iterator.h"

// OWNERSHIP: cart_create allocate memory but is not responsible for freeing it
cart_t *cart_create(size_t id)
{
    cart_t *new_cart = calloc(1, sizeof(cart_t));
    if (new_cart == NULL)
    {
        return NULL;
    }

    new_cart->id = id;
    new_cart->items = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare);

    return new_cart;
}


// NOTE TO SELF: cart's quantity is what the costumer plans to buy
// The stock is what the warehouse/store holds. 
bool add_to_cart(store_t *store, size_t id, char *name, size_t quantity)
{  
    elem_t cart_elem;
    // No cart with this id means there is nothing to add to
    if (!ioopm_hash_table_lookup(store->carts, int_elem(id), &cart_elem))
    {
        return false;
    }

    cart_t *cart = cart_elem.p;

    elem_t merch_elem;
    // Only checks that the name exists; the merch itself is not used here
    if(!ioopm_hash_table_lookup(store->merchandise, string_elem(name), &merch_elem))
    {
        return false;
    }

    merch_t *merch = merch_elem.p;

    // Box for the quantity already in the cart, and the amount to store
    elem_t quantity_elem;
    size_t new_quantity = quantity;

    // If the merch is already in this cart, stack the amounts;
    // adding the same item twice must sum, not replace
    if(ioopm_hash_table_lookup(cart->items, string_elem(name), &quantity_elem))
    {
        // Add total amout of merch in cart
        new_quantity = quantity_elem.i + quantity;
    }

    // Store the total in the cart's own table
    ioopm_hash_table_insert(cart->items, string_elem(name), int_elem(new_quantity));

    return true;
}

// OWNERSHIP: remove_cart did not allocate memory for the items hash_table but is responsible for freeing it 
bool remove_cart(store_t *store, size_t id)
{
    elem_t cart_elem;
    if (!ioopm_hash_table_lookup(store->carts, int_elem(id), &cart_elem))
    {
        return false;
    }

    ioopm_hash_table_remove(store->carts, int_elem(id), &cart_elem);

    cart_t *cart = cart_elem.p;
    ioopm_hash_table_destroy(cart->items);   // the cart owns this table
    free(cart);                              // and the cart struct itself

    return true;
}

bool remove_from_cart(store_t *store, size_t id, char *name, size_t quantity)
{
    elem_t cart_elem;
    if(!ioopm_hash_table_lookup(store->carts, int_elem(id), &cart_elem))
    {
        return false;
    }

    cart_t *cart = cart_elem.p;

    elem_t merch_elem;
    // Only checks that the name exists; the merch itself is not used here
    if(!ioopm_hash_table_lookup(store->merchandise, string_elem(name), &merch_elem))
    {
        return false;
    }

    merch_t *merch = merch_elem.p;

    // The item must already be in this cart, or there is nothing to remove
    elem_t quantity_elem;
    if (!ioopm_hash_table_lookup(cart->items, string_elem(name), &quantity_elem))
    {
        return false;
    }

    size_t in_cart = quantity_elem.i;

    // Cannot remove more than the cart holds
    if (quantity > in_cart)
    {
        return false;
    }

    size_t remaining = in_cart - quantity;

    // Zero items means the merch leaves the cart entirely
    if (remaining == 0)
    {
        ioopm_hash_table_remove(cart->items, string_elem(name), &quantity_elem);
    }
    else
    {
        ioopm_hash_table_insert(cart->items, string_elem(name), int_elem(remaining));
    }

    return true;

}

int calculate_cost(store_t *store, size_t id, char *name, size_t quantitiy)
{
    
}