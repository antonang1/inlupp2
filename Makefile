CFLAGS = -g -Wall -Wextra --coverage

# src
## backend
### Compile:
compile_merch: src/data_structures/hash_table.c src/data_structures/hash_table_iterator.c src/data_structures/linked_list.c src/data_structures/list_iterator.c src/utils/utils.c src/backend/store.c src/backend/stock.c src/backend/merch.c src/tests/test_merch.c
	gcc $(CFLAGS) src/data_structures/hash_table.c src/data_structures/hash_table_iterator.c src/data_structures/linked_list.c src/data_structures/list_iterator.c src/utils/utils.c src/backend/store.c src/backend/stock.c src/backend/merch.c src/tests/test_merch.c -o src/tests/test_merch -lcunit

### Compile and run CUnit tests:

test_merch: compile_merch
	src/tests/test_merch
	valgrind --leak-check=full src/tests/test_merch




clean: 
	find . \( -name "*.gcno" -o -name "*.gcda" -o -name "*.gcov" \) -delete