#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "common.h"

// Typedef ska vara efter struct
typedef struct list ioopm_list_t;
typedef struct list_node ioopm_list_node_t;

typedef void (*elem_destroy_func)(elem_t elem); // LAGT TILL FÖR INLUPP 2, FÖR ATT KUNNA HANTERA ATT ELEM_T KAN VARA EN VOID-PEKARE
// Function pointer used to destroy/free resources owned by an elem_t.
// This is needed when list elements contain dynamically allocated data,
// such as a pointer of type void stored in elem_t.

struct list
{
    ioopm_list_node_t *first;
    ioopm_list_node_t *last;
    size_t size;
    elem_destroy_func destroy;  
};

struct list_node
{
    elem_t head;
    ioopm_list_node_t *tail;
};  

ioopm_list_t *ioopm_list_create(elem_destroy_func destroy)
{
    ioopm_list_t *list = calloc(1, sizeof(ioopm_list_t));
    list->destroy = destroy;

    return list;
}

void ioopm_list_destroy(ioopm_list_t *list)
{
    ioopm_list_node_t *current = list->first;

    while (current != NULL) // Loops through the entire list and frees up the memory of the current node while saving a temporary pointer to the next one for the next iteration
    {
        ioopm_list_node_t *next = current->tail;

        if (list->destroy)
        {
            list->destroy(current->head);
        }

        free(current);
        current = next;
    }

    free(list);
}

void ioopm_list_append(ioopm_list_t *list, elem_t value) // allokerar minne för en nod och kopplar in den sist i listan
{
    ioopm_list_node_t *new_node = calloc(1, sizeof(ioopm_list_node_t));

    new_node->head = value;
    new_node->tail = NULL;

    if (list->size == 0)
    {
        list->first = new_node;
    }
    else
    {
        list->last->tail = new_node;
    }

    list->last = new_node;

    (list->size)++;
}

elem_t ioopm_list_head(ioopm_list_t *list)
{
    return list->first->head;
}

elem_t ioopm_list_last(ioopm_list_t *list)
{
    return list->last->head;
}

void ioopm_list_prepend(ioopm_list_t *list, elem_t value)
{
    ioopm_list_node_t *new_node = calloc(1, sizeof(ioopm_list_node_t));
    new_node->head = value;
    new_node->tail = list->first;
    list->first = new_node;
    (list->size)++;
    if (list->last == NULL)
    {
        list->last = list->first;
    }
}

/// @brief Finds the node before tmp
/// @param tmp First node of the list
/// @param index index of the current node
/// @return The node right before the current one
static ioopm_list_node_t *find_previous(ioopm_list_node_t *tmp, size_t index)
{
    ioopm_list_node_t *current = tmp;
    while (index != 0)
    {
        tmp = current;
        current = current->tail;
        index--;
    }
    return tmp;
}

void ioopm_list_insert(ioopm_list_t *list, size_t index, elem_t value)
{
    if (index == 0)
    {
        ioopm_list_prepend(list, value);
        return;
    }
    else if (index == list->size)
    {
        return;
    }
    ioopm_list_node_t *previous = find_previous(list->first, index);
    ioopm_list_node_t *current = previous->tail;
    //Hittar föregående och nuvaranda nod och skapar den nya noden
    //Sätter sedan in den på rätt ställe
    ioopm_list_node_t *new_node = calloc(1, sizeof(ioopm_list_node_t)); 
    new_node->head = value;
    new_node->tail = current;
    previous->tail = new_node;
    (list->size)++;
}

elem_t ioopm_list_remove(ioopm_list_t *list, size_t index) 
{
    //Hittar föregående och nuvarande nod
    ioopm_list_node_t *previous = find_previous(list->first, index);
    ioopm_list_node_t *current = previous->tail;
    elem_t removed = current->head;
    //Länkar om listan för att frigöra nuvarande nod
    previous->tail = current->tail;
    //Edge cases
    if (index == 0)
    {
        list->first = list->first->tail;
    }
    else if (index == list->size - 1)
    {
        list->last = previous;
    }
    free(current); //Frigör minnet
    (list->size)--;
    return removed;
}

elem_t ioopm_list_get(ioopm_list_t *list, size_t index)
{   //Edge case för när vi vill hämta första elementet en lista
    if (index == 0)
    {
        return list->first->head;
    }
    ioopm_list_node_t *previous = find_previous(list->first, index); //återanvänder gammal funktion
    previous = previous->tail; //Previous blir current
    return previous->head;
}

size_t ioopm_list_size(ioopm_list_t *list)
{
    return list->size;
}

bool ioopm_list_is_empty(ioopm_list_t *list)
{
    return ioopm_list_size(list) == 0;
}
