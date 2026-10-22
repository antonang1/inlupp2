#include "cart.h"
#include "merch.h"
#include "stock.h"
#include "store.h" 
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>
#include "../data_structures/hash_table.h"
#include "../data_structures/hash_table_iterator.h"
#include <stdio.h>

// OWNERSHIP: cart_create allocate memory but is not responsible for freeing it
cart_t *cart_create(size_t id)
{
    cart_t *new_cart = calloc(1, sizeof(cart_t));
    if (new_cart == NULL)
    {
        return NULL;
    }

    new_cart->id = id;
    new_cart->items = ioopm_hash_table_create(ioopm_string_knr_hash, string_compare, NULL);

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
        // Updates the quantity in place if the item already exists in the cart
        ioopm_hash_table_insert(cart->items, string_elem(name), int_elem(remaining));
    }

    return true;

}

int calculate_cost(store_t *store, size_t id)
{
    elem_t cart_elem;
    if (!ioopm_hash_table_lookup(store->carts, int_elem(id), &cart_elem))
    {
        return 0;
    }

    cart_t *cart = cart_elem.p;

    int total = 0;
    
    ioopm_hash_table_iterator_t *cart_iter = ioopm_hash_table_iterator_create(cart->items);

    while (!ioopm_hash_table_iterator_at_end(cart_iter))
    {
        elem_t key = ioopm_hash_table_iterator_current_key(cart_iter);
        elem_t value = ioopm_hash_table_iterator_current_value(cart_iter);
        elem_t result;

        if (ioopm_hash_table_lookup(store_merchandise(store), key, &result))
        {
            merch_t *merch = result.p;
            int price = merch_price(merch);
            int amount = value.i;
            total += price * amount;
        }

        ioopm_hash_table_iterator_advance(cart_iter);   

    }
    ioopm_hash_table_iterator_destroy(cart_iter);
    return total;

}

void stock_decrease_merch(store_t *store, ioopm_hash_table_t *items_of_cart)
{
    ioopm_hash_table_iterator_t *iter = ioopm_hash_table_iterator_create(items_of_cart);
    while (!ioopm_hash_table_iterator_at_end(iter))
    {
        elem_t current_key = ioopm_hash_table_iterator_current_key(iter);
        elem_t current_value = ioopm_hash_table_iterator_current_value(iter);
        
        elem_t merch;
        ioopm_hash_table_lookup(store->merchandise, current_key, &merch);
        
        merch_t *current_merch = merch.p;
        ioopm_list_t *shelfs = current_merch->shelfs;
        ioopm_list_node_t *current_node = shelfs->first;
        
        size_t i = 0;
        while (current_node != NULL)
        {
            shelf_t *current_shelf = current_node->head.p;
            int difference = current_shelf->quantity - current_value.i;
            
            if (difference == 0)
            {   
                // ta bort void pekare mot shelf_t i merch
                ioopm_list_remove(shelfs, i);
                
                elem_t tmp;
                ioopm_hash_table_remove(store->stock, string_elem(current_shelf->shelf_name), &tmp);
                
                break;
            }

            
            if (difference > 0)
            {
                current_shelf->quantity = current_shelf->quantity - current_value.i;

                break;
            }
            
            if (difference < 0)
            {
                int remaining = abs(difference);

                ioopm_list_node_t *next = current_node->tail;
        
                ioopm_list_remove(shelfs, i);

                elem_t tmp;
                ioopm_hash_table_remove(store->stock, string_elem(current_shelf->shelf_name), &tmp);

                current_value.i = remaining;

                current_node = next;
                continue;
            }

        }

    ioopm_hash_table_iterator_advance(iter);
    }

    ioopm_hash_table_iterator_destroy(iter);

    return;
}

bool check_if_all_items_valid(store_t *store, size_t id, ioopm_hash_table_t *items_of_cart, int *status)
{
    //traverse items of cart
    ioopm_hash_table_iterator_t *iter = ioopm_hash_table_iterator_create(items_of_cart);

    while (!ioopm_hash_table_iterator_at_end(iter))
    {
        elem_t current_key = ioopm_hash_table_iterator_current_key(iter);
        elem_t current_value = ioopm_hash_table_iterator_current_value(iter);

        if (!ioopm_hash_table_has_key(store->merchandise, current_key))
        {

            remove_from_cart(store, id, current_key.s, (size_t) current_value.i);

            *status = -2; //-2 indicates that merch in cart does not exist in store anymore!
            ioopm_hash_table_iterator_destroy(iter);
            return false; 
        }

        elem_t merch;
        ioopm_hash_table_lookup(store->merchandise, current_key, &merch);
        
        merch_t *current_merch = merch.p;
        ioopm_list_t *shelfs = current_merch->shelfs;
        ioopm_list_node_t *current_node = shelfs->first;

        size_t total_stock = 0;

        while (current_node != NULL)
        {
            shelf_t *current_shelf = current_node->head.p;

            total_stock += current_shelf->quantity;

            current_node = current_node->tail;
        }

        if (current_value.i > (int) total_stock)
        {
            remove_from_cart(store, id, current_key.s, (size_t) current_value.i);
            
            *status = -3; // -3 indicates that the user tries to buy more items than allowed.
            ioopm_hash_table_iterator_destroy(iter);
            return false; 
        }
        
        ioopm_hash_table_iterator_advance(iter);
    }
    *status = 0;
    ioopm_hash_table_iterator_destroy(iter);
    return true;

}

int checkout(store_t *store, size_t id)
{
    elem_t cart_elem;
    if (!ioopm_hash_table_lookup(store->carts, int_elem(id), &cart_elem))
    {
        return -1; // -1 indicates that cart does not exist. 
    }
    // Decrease the stock for the merches in the cart.
    
    cart_t *cart = cart_elem.p;
    ioopm_hash_table_t *items_of_cart = cart->items;

    if (cart == NULL)
    {
        remove_cart(store, id);
        return 0;
    }
    
    if (items_of_cart == NULL)
    {
        remove_cart(store, id);
        return 0;
    }

    
    int status;
    if (!check_if_all_items_valid(store, id, items_of_cart, &status))
    {
        return status;
    }
    
    stock_decrease_merch(store, items_of_cart);

    int total = calculate_cost(store, id);

    remove_cart(store, id);
    return total;

}