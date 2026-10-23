CFLAGS = -g -Wall -Wextra --coverage

# Shared source files for both test suites
SRC = src/data_structures/hash_table.c \
      src/data_structures/hash_table_iterator.c \
      src/data_structures/linked_list.c \
      src/data_structures/list_iterator.c \
      src/utils/utils.c \
      src/backend/store.c \
      src/backend/stock.c \
      src/backend/merch.c \
      src/backend/cart.c \
	  src/ui/ui.c \
	  main.c

# --- Merch tests ---
compile_merch: $(SRC) src/tests/test_merch.c
	gcc $(CFLAGS) $(SRC) src/tests/test_merch.c -o src/tests/test_merch -lcunit

test_merch: compile_merch
	src/tests/test_merch
	valgrind --leak-check=full src/tests/test_merch

# --- Cart tests ---
compile_cart: $(SRC) src/tests/test_cart.c
	gcc $(CFLAGS) $(SRC) src/tests/test_cart.c -o src/tests/test_cart -lcunit

test_cart: compile_cart
	src/tests/test_cart
	valgrind --leak-check=full src/tests/test_cart

# --- Stock tests ---
compile_stock: $(SRC) src/tests/test_stock.c
	gcc $(CFLAGS) $(SRC) src/tests/test_stock.c -o src/tests/test_stock -lcunit

test_stock: compile_stock
	src/tests/test_stock
	valgrind --leak-check=full src/tests/test_stock

# --- Store tests ---
compile_store: $(SRC) src/tests/test_store.c
	gcc $(CFLAGS) $(SRC) src/tests/test_store.c -o src/tests/test_store -lcunit

test_store: compile_store
	src/tests/test_store
	valgrind --leak-check=full src/tests/test_store

# --- Run all suites ---
test: test_merch test_cart test_stock test_store

compile_main: $(SRC)
	gcc $(CFLAGS) $(SRC) -o main

clean:
	find . \( -name "*.gcno" -o -name "*.gcda" -o -name "*.gcov" \) -delete