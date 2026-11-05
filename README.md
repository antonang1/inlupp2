# inlupp2

Simple webstore backend with a text user interface, built on the data
structures from assignment 1.

## USE OF AI

This README was written drafted loosly with the help of KLAR in order to structure and phrase the text. The draft was then
rewritten and reviewed by Signe von Greiff and Anton Äng to accuratley describe the code written. After the README was completed, KLAR was used to spellcheck the text without rewriting any of the text.   

## Building and running the tests

make test

Builds and runs the four CUnit suites in order: test_merch, test_cart,
test_stock and test_store. Each suite is run once normally and then once
under valgrind --leak-check=full.

A single suite can be run on its own:

make test_merch

Compilation uses gcc with -g -Wall -Wextra --coverage. The program is
written for a UNIX environment.

## Testing

The four suites test the backend functions: merchandise, storage
locations, shopping carts and stock. The test files live in src/tests.
Each test file builds its own store and checks both valid and invalid
input, for example that an item that already exists cannot be added
again, and that a storage location holding a different item cannot be
replenished. Calls that go through the hash table and the linked list
are not tested separately: those come from assignment 1 and have their
own test suites.

The user interface is not covered by the unit tests.

## Running the program

The program is built and started with:

make compile_main
./main

It enters an event loop and reads one action code at a time from
standard input. A confirmation is asked for before anything is removed
or changed, and the action is carried out only if the answer begins with
Y or y.

The action codes are:

| Code | Action              | Reads                                                       |
|------|---------------------|-------------------------------------------------------------|
| A    | Add merchandise     | name, description, price                                    |
| L    | List merchandise    | nothing; prints 20 at a time                                |
| D    | Remove merchandise  | name, confirmation                                          |
| E    | Edit merchandise    | old name, new name, new description, new price, confirmation|
| S    | Show stock          | name of the merchandise                                     |
| P    | Replenish           | storage location, name of the merchandise, quantity         |
| C    | Create cart         | nothing                                                     |
| R    | Remove cart         | cart id, confirmation                                       |
| +    | Add to cart         | cart id, name of the merchandise, quantity                  |
| -    | Remove from cart    | cart id, name of the merchandise, quantity                  |
| =    | Calculate cost      | cart id                                                     |
| O    | Checkout            | cart id                                                     |
| Q    | Quit                | confirmation                                                |

Lower case letters are accepted. Every item of data is read on its own
line, in the order given above. The format is checked as soon as the
data has been read, and on bad input the program asks for that value
again.

## Testing the interface

The course supplies a script that runs through a sequence of actions:
make compile_main
./main < webstoretest.txt



## Module layout

| Directory        | Contents                                                |
|------------------|---------------------------------------------------------|
| data_structures  | hash table and linked list, from assignment 1           |
| utils            | input reading, conversion, sorting and hashing          |
| backend          | merch.c, cart.c, stock.c and store.c                    |
| ui               | menu, action codes and printing                         |
| tests            | CUnit tests, one file per module under test             |

Each module is a pair of.c and.h files. The header file is the interface
the other modules are allowed to use.

## Where the components came from

This is a pair assignment, and the components from earlier work come
from more than one person.

| Component   | Source                       |
|-------------|------------------------------|
| Hash table  | assignment 1, Anton          |
| Linked list | assignment 1, Anton          |
| Utils       | bootstrap labs, Signe        |

## Code coverage

Tool: gcov, through the --coverage flag

| File       | Line coverage | Branch coverage |
|------------|---------------|-----------------|
| merch.c    | 52.21%        | 51.79%          |
| cart.c     | 33.68%        | 27.88%          |
| stock.c    | 66.10%        | 72.22%          |
| store.c    | 76.19%        | 87.50%          |
| utils.c    | 16.07%        | 7.81%           |

| test_merch.c    | 98.26%   | 100.0%          |
| test_cart.c     | 97.75%%  | 100.0%          |
| test_stock.c    | 97.40%   | 100.0%          |
| test_store.c    | 95.41%   | 100.0%          |

The user interface is not covered by the unit tests. The numbers come
from gcov and are filled in from a run of your own.

## Documentation

The public functions in utils and the backend modules are documented
with Doxygen comments in their header files.

## Design

### Data structures

Two things were changed in the hash table from assignment 1. First, the
struct gained a destroy function per table, instead of every caller
freeing its own values. The table therefore takes care of cleaning up
its own keys and values when it is destroyed, and store_destroy only has
to destroy three tables. Each table is created with a callback for its
value type, which frees the value and what that value owns.

Second, ioopm_hash_table_remove was changed so that it writes the
removed value to its out-parameter instead of only reporting that
something was removed. That is needed in the places where the value is
used afterwards, for example when an item is renamed and has to be
inserted under its new key.

The linked list also gained a destroy function. It is in place but not
fully used yet because every storage location is owned by the store and
is freed through the store's own callback. It stays so that the list
remains complete as a generic data structure.

### Modules

The program is divided into five directories. data_structures and utils
are generic and know nothing about merchandise or storage locations,
while backend and ui are specific to this program. The ui handles menu
choices and reading the program's own data, while the generic input
routines, such as ask_question_string, stay in utils where they belong
and can be used by any program.

The backend is in turn divided into merch.c, cart.c, stock.c and
store.c. The store owns the three tables and is the only one that
creates and destroys them. merch and stock keep track of merchandise and
storage locations, and cart keeps track of shopping carts. Each module
has a header pair that is the only surface the others may use, which
shows in cart.c reading a price through merch_price instead of knowing
how merch_t stores it.

### Two tables kept in sync

The store has two hash tables. merchandise maps merchandise names to the
item's information, and stock maps storage location names to the storage
location. The first makes it fast to go from a name to an item and its
storage locations, the second makes it fast to go from a storage
location to what is stored on it.

The price of that design is that the two must be kept in sync, and this
shows most clearly on a rename. When an item is renamed it is moved in
merchandise by being extracted and inserted again, and after that the new
name is copied out onto every storage location holding the item. Forget
the second half and the storage location points at a name that no longer
exists in the store.

### Input

The input handling follows the same pattern as the database
assignments: a question, a check function and a conversion function. The
check runs as soon as the line has been read, so that a wrong format is
caught before the rest of the command has been parsed, and the question
is asked again until the answer is acceptable. That lets ui.c ask for a
storage location with ask_question_shelf and get back a value that has
already been checked.

### Ownership

The rule throughout the program is that whoever creates something owns
it, and the ownership is written out in the header. The store owns the
three tables, the tables own their values through their destroy
callbacks, and every item owns its own list of storage location
pointers. Whoever gets an array back owns only the array: list_shelves
returns an array that the caller frees, but the storage locations
themselves belong to the store and must not be freed.