#pragma once
#include "../backend/store.h"
#include "../backend/merch.h"
#include "../backend/stock.h"
#include "../backend/cart.h"
#include <stdio.h>
#include <ctype.h>

/* 
NOTE: Documentation outline generated with KLAR, then rewritten and verified by Signe von Greiff and Anton Äng.
No code in this repository was generated.
*/

/// @brief Runs the menu loop, executing the user's chosen command until
///        the user chooses to quit.
/// @param store The store every command reads from and writes to.
/// @return Always 1. The value signals that the loop has ended, not
///         whether anything succeeded.
/// @note Dispatches the upper-cased menu character through an if/else
///       chain against A, L, D, E, S, P, C, R, U, +, -, =, O and Q.
/// @note Only Q ends the loop, by setting run to false. Nothing else
///       inside the loop can leave it.
int event_loop(store_t *store);