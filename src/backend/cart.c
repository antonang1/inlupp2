#include "cart.h"
#include "store.h"
#include "merch.h"
#include "stock.h"

// OWNERSHIP: ioopm_cart_create allocate memory but is not responsible for freeing it
cart_t *ioopm_cart_create(size_t id)
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


void ioopm_add_cart_to_store(store_t *store, cart_t *cart)
{
    ioopm_hash_table_insert(store->carts, int_elem(cart->id), ptr_elem(cart));

    store->next_cart_id++;

    return;
}

size_t ioopm_get_occupied_stock(store_t *store, merch_t *merch)
{
    size_t total = 0;
    ioopm_hash_table_iterator_t *iter_over_carts = ioopm_hash_table_iterator_create(store->carts);

    while (!ioopm_hash_table_iterator_at_end(iter_over_carts))
    {
        elem_t current_value = ioopm_hash_table_iterator_current_value(iter_over_carts);
        cart_t *current_cart = current_value.p;

        ioopm_hash_table_iterator_t *iter_inside_cart = ioopm_hash_table_iterator_create(current_cart->items);

        while (!ioopm_hash_table_iterator_at_end(iter_inside_cart))
        {
            elem_t current_key_inside = ioopm_hash_table_iterator_current_key(iter_inside_cart);
            elem_t current_value_inside = ioopm_hash_table_iterator_current_value(iter_inside_cart);

            if (strcmp(current_key_inside.s, merch->name) == 0)
            {
                total += current_value_inside.i;
            }
            
            ioopm_hash_table_iterator_advance(iter_inside_cart);
        }
        ioopm_hash_table_iterator_destroy(iter_inside_cart);
        
        ioopm_hash_table_iterator_advance(iter_over_carts);
    }

    ioopm_hash_table_iterator_destroy(iter_over_carts);

    return total;
}
    
bool ioopm_add_to_cart(store_t *store, size_t id, char *name, size_t quantity)
{
    elem_t cart_found;
    elem_t merch_found;
    
    if (!ioopm_hash_table_lookup(store->carts, int_elem(id), &cart_found)
        || !ioopm_hash_table_lookup(store->merchandise, string_elem(name), &merch_found)) { return false; }

    //cart exists, merch exists
    cart_t *cart_of_intrest = cart_found.p;
    merch_t *merch_of_intrest = merch_found.p;

    size_t total_stock = ioopm_get_total_stock(merch_of_intrest);
    size_t stock_occupied_by_carts = ioopm_get_occupied_stock(store, merch_of_intrest);
    size_t available_stock = total_stock - stock_occupied_by_carts;


    if (quantity > available_stock) { return false; } // there is not enough stock.
    
    elem_t prev_amount;
    if (ioopm_hash_table_lookup(cart_of_intrest->items, string_elem(name), &prev_amount))
    {
        prev_amount.i += quantity;
        ioopm_hash_table_insert(cart_of_intrest->items, string_elem(name), int_elem(prev_amount.i));
        return true;
    }

    char *name_copy = strdup(name);
    ioopm_hash_table_insert(cart_of_intrest->items, string_elem(name_copy), int_elem(quantity));
    return true;
}

// OWNERSHIP: ioopm_remove_cart did not allocate memory for the items hash_table but is responsible for freeing it 
bool ioopm_remove_cart(store_t *store, size_t id)
{
    elem_t cart_elem;
    if (!ioopm_hash_table_lookup(store->carts, int_elem(id), &cart_elem))
    {
        return false;
    }

    ioopm_hash_table_remove(store->carts, int_elem(id), &cart_elem);

    return true;
}

bool ioopm_remove_from_cart(store_t *store, size_t id, char *name, size_t quantity)
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
        // each key in cart->items is a allocated string with stdup(), therefore we need to gather that key, save it before removal and then free it.
        char *key_to_free = NULL;

        ioopm_hash_table_iterator_t *iter = ioopm_hash_table_iterator_create(cart->items);

        while (!ioopm_hash_table_iterator_at_end(iter))
        {
            elem_t current_key = ioopm_hash_table_iterator_current_key(iter);       

            if (strcmp(current_key.s, name) == 0)
            {
                key_to_free = current_key.s;
                break;
            }

            ioopm_hash_table_iterator_advance(iter);
        }

        ioopm_hash_table_iterator_destroy(iter);
        
        elem_t removed_value;
        
        ioopm_hash_table_remove(cart->items, string_elem(name), &removed_value);
        free(key_to_free);
    }
    else
    {
        // Updates the quantity in place if the item already exists in the cart
        ioopm_hash_table_insert(cart->items, string_elem(name), int_elem(remaining));
    }

    return true;

}

int ioopm_calculate_cost(store_t *store, size_t id)
{
    elem_t cart_elem;
    if (!ioopm_hash_table_lookup(store->carts, int_elem(id), &cart_elem))
    {
        return 0;
    }

    cart_t *cart = cart_elem.p;

    size_t total = 0;
    
    ioopm_hash_table_iterator_t *cart_iter = ioopm_hash_table_iterator_create(cart->items);

    if (cart_iter == NULL)
    {
        return 0;
    }

    while (!ioopm_hash_table_iterator_at_end(cart_iter))
    {
        elem_t key = ioopm_hash_table_iterator_current_key(cart_iter);
        elem_t value = ioopm_hash_table_iterator_current_value(cart_iter);
        elem_t result;

        if (ioopm_hash_table_lookup(ioopm_store_merchandise(store), key, &result))
        {
            merch_t *merch = result.p;
            size_t price = (size_t) ioopm_merch_price(merch);
            size_t amount = (size_t) value.i;
            total += price * amount;
        }

        ioopm_hash_table_iterator_advance(cart_iter);   

    }
    ioopm_hash_table_iterator_destroy(cart_iter);
    return total;

}

void ioopm_stock_decrease_merch(store_t *store, ioopm_hash_table_t *items_of_cart)
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

bool ioopm_check_if_all_items_valid(store_t *store, ioopm_hash_table_t *items_of_cart)
{
    if (store == NULL || items_of_cart == NULL)
    {
        return false;
    }

    ioopm_hash_table_iterator_t *iter = ioopm_hash_table_iterator_create(items_of_cart);

    if (iter == NULL)
    {
        return false;
    }

    while (!ioopm_hash_table_iterator_at_end(iter))
    {
        elem_t current_key = ioopm_hash_table_iterator_current_key(iter);

        elem_t current_value = ioopm_hash_table_iterator_current_value(iter);

        // Check that the merchandise still exists.
        elem_t merch_elem;
        if (!ioopm_hash_table_lookup(store->merchandise, current_key, &merch_elem))
        {
            ioopm_hash_table_iterator_destroy(iter);
            return false;
        }

        merch_t *current_merch = merch_elem.p;

        size_t total_stock = ioopm_get_total_stock(current_merch);

        // Check whether the cart can still be fullfilled.
        if ((size_t) current_value.i > total_stock)
        {
            ioopm_hash_table_iterator_destroy(iter);
            return false;
        }

        ioopm_hash_table_iterator_advance(iter);
    }

    ioopm_hash_table_iterator_destroy(iter);
    return true;
}

bool ioopm_checkout(store_t *store, size_t id)
{
    elem_t cart_elem;
    if (!ioopm_hash_table_lookup(store->carts, int_elem(id), &cart_elem))
    {
        return false; 
    }
    
    // Decrease the stock for the merches in the cart.
    cart_t *cart = cart_elem.p;
    ioopm_hash_table_t *items_of_cart = cart->items;

    if (cart == NULL)
    {
        ioopm_remove_cart(store, id);
        return true;
    }
    
    if (items_of_cart == NULL)
    {
        ioopm_remove_cart(store, id);
        return true;
    }

    if (!ioopm_check_if_all_items_valid(store, items_of_cart))
    {
        return false;
    }
    
    ioopm_stock_decrease_merch(store, items_of_cart);

    ioopm_remove_cart(store, id);

    return true;

}