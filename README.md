# inlupp2
ioopm inlupp2 anton signe

#K30
A program should be divided into smaller modules rather than written as one monolithic block, partly because it lets several programmers work on different parts at the same time. The division must be natural, and each module needs a clear interface that the other modules program against. The program is organised into directories such as backend, data_structures, ui, utils and tests. The actual modules live inside these directories: merch.c, cart.c and store.c in backend, the hash table in data_structures, and the menu in ui. Each module is a pair of.c and.h files, and the header file is the interface the other modules are allowed to use.

#K31
Low coupling means two modules should only be loosely connected, so that one can be swapped out without real changes to the other, and high cohesion means every part inside a module should concern the same aspect of the program. These two properties together are what make a program well modularised. In this program, cart.c has high cohesion because everything in it deals with shopping carts, and it has low coupling towards merch because it reads a price through merch_price instead of knowing how merch_t stores that price.

#K32
Separation of concerns means keeping the different responsibilities a program has apart, so that a change in one of them does not force changes in the others. It is the principle that produces the two properties from K31: if the program is divided along the responsibilities that actually belong together, each module gets high cohesion and the interfaces between them become few and simple. In this program, menu handling and input live in ui.c while the calculation lives in calculate_cost in cart.c, and merch_price separates the responsibility of knowing how a merch stores its price from the responsibility of summing up a cart. A negative example is the loop in calculate_cost, which at the same time reads the iterator, performs the table lookup and computes the price.

Keeping the price lookup in merch_price instead of reading merch->price directly in calculate_cost separates the job of knowing how a merch stores its price from the job of adding up a cart, so the two can change independently.

// backend -> merch.c -> cart.c -> calculate_cost