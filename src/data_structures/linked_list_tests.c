#include <CUnit/Basic.h>
#include "linked_list.h"
#include "list_iterator.h"
#include "common.h"
#include <stdlib.h>

typedef union element elem_t;

int init_suite(void)
{
  // Change this function if you want to do something *before* you
  // run a test suite
  return 0;
}

int clean_suite(void)
{
  // Change this function if you want to do something *after* you
  // run a test suite
  return 0;
}

// These are example test functions. You should replace them with
// functions of your own.
void test_empty(void)
{
  ioopm_list_t *listan = ioopm_list_create(NULL);
  CU_ASSERT_EQUAL(NULL, listan->first);
  CU_ASSERT_EQUAL(NULL, listan->last);
  CU_ASSERT_EQUAL(listan->size, 0);
  ioopm_list_destroy(listan);
}

void test_singleton(void)
{
  ioopm_list_t *listan = ioopm_list_create(NULL);
  ioopm_list_append(listan, (elem_t)67);
  CU_ASSERT_EQUAL(listan->first->head.i, 67);
  CU_ASSERT_EQUAL(listan->last->head.i, 67);
  CU_ASSERT_EQUAL(listan->size, 1);
  ioopm_list_destroy(listan);
}

void test_3_appends(void)
{
  ioopm_list_t *listan = ioopm_list_create(NULL);
  ioopm_list_append(listan, (elem_t)67);
  ioopm_list_append(listan, (elem_t)68);
  ioopm_list_append(listan, (elem_t)69);

  CU_ASSERT_EQUAL(listan->first->head.i, 67);
  CU_ASSERT_EQUAL(listan->last->head.i, 69);
  CU_ASSERT_EQUAL(listan->size, 3);
  CU_ASSERT_EQUAL(listan->first->tail->head.i, 68);

  ioopm_list_destroy(listan);
}

/* deepseek */
void test_ioopm_list_create(void)
{
  ioopm_list_t *list = ioopm_list_create(NULL);
  CU_ASSERT_PTR_NOT_NULL(list);
  CU_ASSERT_EQUAL(list->size, 0);
  CU_ASSERT_PTR_NULL(list->first);
  CU_ASSERT_PTR_NULL(list->last);
  ioopm_list_destroy(list);
}

/* deepseek*/
void test_ioopm_list_size(void)
{
  ioopm_list_t *list = ioopm_list_create(NULL);
  CU_ASSERT_EQUAL(ioopm_list_size(list), 0);

  elem_t v1 = {.i = 10};
  ioopm_list_append(list, v1);
  CU_ASSERT_EQUAL(ioopm_list_size(list), 1);

  elem_t v2 = {.i = 20};
  ioopm_list_append(list, v2);
  CU_ASSERT_EQUAL(ioopm_list_size(list), 2);

  ioopm_list_destroy(list);
}

/* deepseek */
void test_ioopm_list_is_empty(void)
{
  ioopm_list_t *list = ioopm_list_create(NULL);
  CU_ASSERT_TRUE(ioopm_list_is_empty(list));

  elem_t v = {.i = 5};
  ioopm_list_append(list, v);
  CU_ASSERT_FALSE(ioopm_list_is_empty(list));

  ioopm_list_destroy(list);
}

/* deepseek */
void test_ioopm_list_prepend(void)
{
  ioopm_list_t *list = ioopm_list_create(NULL);

  elem_t v1 = {.i = 10};
  ioopm_list_prepend(list, v1);
  CU_ASSERT_PTR_NOT_NULL(list->first);
  CU_ASSERT_EQUAL(list->first->head.i, 10);
  CU_ASSERT_PTR_NULL(list->first->tail);

  elem_t v2 = {.i = 20};
  ioopm_list_prepend(list, v2);
  CU_ASSERT_EQUAL(list->first->head.i, 20);
  CU_ASSERT_EQUAL(list->first->tail->head.i, 10);

  ioopm_list_destroy(list);
}

/* deepseek */
void test_ioopm_list_head(void)
{
  ioopm_list_t *list = ioopm_list_create(NULL);

  elem_t v1 = {.i = 10};
  ioopm_list_append(list, v1);

  elem_t v2 = {.i = 20};
  ioopm_list_append(list, v2);

  elem_t head = ioopm_list_head(list);
  CU_ASSERT_EQUAL(head.i, 10);

  ioopm_list_destroy(list);
}

/* deepseek */
void test_ioopm_list_last(void)
{
  ioopm_list_t *list = ioopm_list_create(NULL);

  elem_t v1 = {.i = 10};
  ioopm_list_append(list, v1);

  elem_t v2 = {.i = 20};
  ioopm_list_append(list, v2);

  elem_t last = ioopm_list_last(list);
  CU_ASSERT_EQUAL(last.i, 20);

  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);
  while (!ioopm_list_iterator_at_end(it))
  {
    ioopm_list_iterator_advance(it);
  }
  elem_t last2 = ioopm_list_iterator_current(it);
  CU_ASSERT_EQUAL(last2.i, 20);

  ioopm_list_iterator_destroy(it);
  ioopm_list_destroy(list);
}

/* deepseek */
void test_ioopm_list_insert(void)
{
  ioopm_list_t *list = ioopm_list_create(NULL);

  elem_t v1 = {.i = 10};
  elem_t v2 = {.i = 20};
  elem_t v3 = {.i = 30};

  /* Infoga vid index 0 (ska prependa) */
  ioopm_list_insert(list, 0, v1);
  CU_ASSERT_EQUAL(list->first->head.i, 10);

  ioopm_list_insert(list, 0, v2);
  CU_ASSERT_EQUAL(list->first->head.i, 20);
  CU_ASSERT_EQUAL(list->first->tail->head.i, 10);

  /* Infoga vid index 1 (mellan två noder) */
  ioopm_list_insert(list, 1, v3);
  elem_t a2 = ioopm_list_get(list, 2);
  elem_t a1 = ioopm_list_get(list, 1);
  elem_t a0 = ioopm_list_get(list, 0);

  CU_ASSERT_EQUAL(a0.i, 20);
  CU_ASSERT_EQUAL(a1.i, 30);
  CU_ASSERT_EQUAL(a2.i, 10);

  ioopm_list_destroy(list);
}

/* deepseek */
void test_ioopm_list_get(void)
{
  ioopm_list_t *list = ioopm_list_create(NULL);

  elem_t v1 = {.i = 10};
  elem_t v2 = {.i = 20};
  elem_t v3 = {.i = 30};

  ioopm_list_append(list, v1);
  ioopm_list_append(list, v2);
  ioopm_list_append(list, v3);

  CU_ASSERT_EQUAL(ioopm_list_get(list, 0).i, 10);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 1).i, 20);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 2).i, 30);

  ioopm_list_destroy(list);
}

/* deepseek men uppdaterat till iterator av mig*/
void test_ioopm_list_remove(void)
{
  ioopm_list_t *list = ioopm_list_create(NULL);

  elem_t v1 = {.i = 10};
  elem_t v2 = {.i = 20};
  elem_t v3 = {.i = 30};

  ioopm_list_append(list, v1);
  ioopm_list_append(list, v2);
  ioopm_list_append(list, v3);

  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);
  ioopm_list_iterator_advance(it);

  elem_t removed = ioopm_list_iterator_remove(it);
  CU_ASSERT_EQUAL(removed.i, 20);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 0).i, 10);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 1).i, 30);

  ioopm_list_iterator_destroy(it);
  ioopm_list_destroy(list);
}

void test_iterator_iterate(void)
{ 
  ioopm_list_t *list = ioopm_list_create(NULL);
  elem_t v1 = {.i = 10};
  elem_t v2 = {.i = 20};
  elem_t v3 = {.i = 30};
  ioopm_list_append(list, v1);
  ioopm_list_append(list, v2);
  ioopm_list_append(list, v3);
  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);

  CU_ASSERT_FALSE(ioopm_list_iterator_at_end(it)); // på 10
  CU_ASSERT_EQUAL(ioopm_list_iterator_current(it).i, 10);

  ioopm_list_iterator_advance(it); // på 20
  CU_ASSERT_FALSE(ioopm_list_iterator_at_end(it));
  CU_ASSERT_EQUAL(ioopm_list_iterator_current(it).i, 20);

  ioopm_list_iterator_remove(it); // på 30
  CU_ASSERT_TRUE(ioopm_list_iterator_at_end(it));
  CU_ASSERT_EQUAL(ioopm_list_iterator_current(it).i, 30);

  ioopm_list_iterator_insert(it, v2);             // sätter in 20 igen
  CU_ASSERT_TRUE(ioopm_list_iterator_at_end(it)); 
  CU_ASSERT_EQUAL(ioopm_list_iterator_current(it).i, 30);

  ioopm_list_iterator_destroy(it);
  ioopm_list_destroy(list);
}

void test_iterator_insert(void)
{
  ioopm_list_t *list = ioopm_list_create(NULL);
  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);

  elem_t v1 = {.i = 10};
  elem_t v2 = {.i = 20};
  elem_t v3 = {.i = 30};

  ioopm_list_iterator_insert(it, v1); // insertar på index 0
  ioopm_list_iterator_insert(it, v2);
  ioopm_list_iterator_insert(it, v3);

  CU_ASSERT_EQUAL(ioopm_list_iterator_current(it).i, 10);

  ioopm_list_iterator_advance(it);
  CU_ASSERT_EQUAL(ioopm_list_iterator_current(it).i, 20);

  ioopm_list_iterator_advance(it);
  CU_ASSERT_EQUAL(ioopm_list_iterator_current(it).i, 30);

  ioopm_list_iterator_destroy(it);
  ioopm_list_destroy(list);
}



typedef struct test_shelf
{
  char *name;
  size_t size;
} test_shelf_t;

void destroy_test_shelf(elem_t elem)
{
  test_shelf_t *shelf = elem.p;

  free(shelf->name);
  free(shelf);
}

void test_void_pointer_elements(void)
{
  ioopm_list_t *list = ioopm_list_create(destroy_test_shelf);

  test_shelf_t *shelf1 = malloc(sizeof(test_shelf_t));
  shelf1->name = malloc(10);
  strcpy(shelf1->name, "Shelf 1");
  shelf1->size = 10;

  test_shelf_t *shelf2 = malloc(sizeof(test_shelf_t));
  shelf2->name = malloc(10);
  strcpy(shelf2->name, "Shelf 2");
  shelf2->size = 20;

  ioopm_list_append(list, (elem_t){.p = shelf1});
  ioopm_list_append(list, (elem_t){.p = shelf2});

  CU_ASSERT_EQUAL(list->size, 2);

  test_shelf_t *first = ioopm_list_get(list, 0).p;
  test_shelf_t *last = ioopm_list_get(list, 1).p;

  CU_ASSERT_STRING_EQUAL(first->name, "Shelf 1");
  CU_ASSERT_EQUAL(first->size, 10);

  CU_ASSERT_STRING_EQUAL(last->name, "Shelf 2");
  CU_ASSERT_EQUAL(last->size, 20);

  ioopm_list_destroy(list);
}


int main()
{
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite my_test_suite = CU_add_suite("My awesome test suite", init_suite, clean_suite);
  if (my_test_suite == NULL)
  {
    // If the test suite could not be added, tear down CUnit and exit
    CU_cleanup_registry();
    return CU_get_error();
  }

  // This is where we add the test functions to our test suite.
  // For each call to CU_add_test we specify the test suite, the
  // name or description of the test, and the function that runs
  // the test in question. If you want to add another test, just
  // copy a line below and change the information
  if (
      (CU_add_test(my_test_suite, "Empty list", test_empty) == NULL)
      || (CU_add_test(my_test_suite, "Singleton append", test_singleton) == NULL)
      || (CU_add_test(my_test_suite, "3 entry list append", test_3_appends) == NULL)
      || (CU_add_test(my_test_suite, "ioopm_list_create", test_ioopm_list_create) == NULL)
      || (CU_add_test(my_test_suite, "ioopm_list_size", test_ioopm_list_size) == NULL)
      || (CU_add_test(my_test_suite, "ioopm_list_is_empty", test_ioopm_list_is_empty) == NULL)
      || (CU_add_test(my_test_suite, "ioopm_list_prepend", test_ioopm_list_prepend) == NULL)
      || (CU_add_test(my_test_suite, "ioopm_list_head", test_ioopm_list_head) == NULL)
      || (CU_add_test(my_test_suite, "ioopm_list_last", test_ioopm_list_last) == NULL)
      || (CU_add_test(my_test_suite, "ioopm_list_insert", test_ioopm_list_insert) == NULL)
      || (CU_add_test(my_test_suite, "ioopm_list_get", test_ioopm_list_get) == NULL)
      || (CU_add_test(my_test_suite, "ioopm_list_remove", test_ioopm_list_remove) == NULL)
      || (CU_add_test(my_test_suite, "ioopm_list_iterator", test_iterator_iterate) == NULL)
      || (CU_add_test(my_test_suite, "iterator_insert", test_iterator_insert) == NULL)
      || (CU_add_test(my_test_suite, "void pointer elements", test_void_pointer_elements) == NULL))
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
