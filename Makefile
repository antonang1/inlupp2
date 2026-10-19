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
      src/backend/cart.c

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

# --- Run both suites ---
test: test_merch test_cart

clean:
	find . \( -name "*.gcno" -o -name "*.gcda" -o -name "*.gcov" \) -delete