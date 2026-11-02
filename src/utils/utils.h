#pragma once
#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdbool.h>
#include <stdlib.h>
#include "../data_structures/common.h"


extern char *strdup(const char *);

typedef union {
  int   int_value;
  float float_value;
  char *string_value;
} answer_t;

/// @brief The function type used to validate input before it is converted.
/// @param str The trimmed input string.
/// @return true if the input is acceptable, false to ask again.
typedef bool check_func(char *);

/// @brief The function type used to turn accepted input into an answer_t.
/// @param str The trimmed input string, already accepted by the check.
/// @return The input converted to the right member of answer_t.
typedef answer_t convert_func(char *);

/// @brief The function type used to compare two elem_t for sorting.
/// @param a The first element.
/// @param b The second element.
/// @return Negative if a sorts before b, zero if they are equal,
///         positive if a sorts after b.
typedef int (*comparison_func)(elem_t, elem_t);

/// @brief Reads characters into buf until a newline or buf_siz - 1 chars.
/// @param buf The buffer to write into.
/// @param buf_siz The size of the buffer, including the terminator.
/// @return The number of characters read, not counting the newline.
/// @note Always null-terminates the buffer. If the line is longer than
///       the buffer, the rest is drained with clear_input_buffer.
int read_string(char *buf, int buf_siz);

/// @brief Removes leading and trailing whitespace from a string.
/// @param str The string to trim. Modified in place.
/// @return The same pointer that was passed in.
/// @note Shifts the kept characters to the front and null-terminates.
char *trim(char *str);

/// @brief Prints a question and reads input until check accepts it.
/// @param question The text to print.
/// @param check The function that validates the input.
/// @param convert The function that turns the accepted string into an
///        answer_t.
/// @return The converted answer.
/// @note Loops until check returns true, so blank or malformed input is
///       never passed on to convert.
answer_t ask_question(char *question, check_func *check, convert_func *convert);

/// @brief Checks whether a string is a valid integer, sign included.
/// @param str The string to check.
/// @return true if str is a non-empty sequence of digits with an optional
///         leading minus. A lone minus is not accepted.
bool is_number(char *str);

/// @brief Checks whether a string is a valid non-negative integer.
/// @param str The string to check.
/// @return true if str is a non-empty sequence of digits. Zero counts as
///         positive here.
bool is_positive_number(char *str);

/// @brief Asks for an integer and validates it with check.
/// @param question The text to print.
/// @param check The function that validates the input.
/// @return The value as an int.
int ask_question_int(char *question, check_func *check);

/// @brief Turns a string into a float wrapped in an answer_t.
/// @param str The string to convert.
/// @return An answer_t holding the parsed float value.
answer_t make_float(char *str);

/// @brief Checks whether a string is a valid float.
/// @param str The string to check.
/// @return true if str has digits with exactly one decimal point and an
///         optional leading minus. A string without a point is not
///         accepted.
bool is_float(char *str);

/// @brief Asks for a float and validates it with is_float.
/// @param question The text to print.
/// @return The value as a double.
double ask_question_float(char *question);

/// @brief Checks that a string is not empty.
/// @param str The string to check.
/// @return true if str has at least one character.
bool not_empty(char *str);

/// @brief Asks for a non-empty string.
/// @param question The text to print.
/// @return A heap-allocated copy of the answer. The caller must free it.
/// @note Copies the input with strdup, so the read buffer's lifetime does
///       not matter.
char *ask_question_string(char *question);

/// @brief Checks whether a string is a valid shelf name.
/// @param str The string to check.
/// @return true if str starts with a letter followed by at least one
///         digit, for example "A12". At least two characters are required.
bool is_shelf(char *str);

/// @brief Asks for a shelf name and validates it with is_shelf.
/// @param question The text to print.
/// @return A heap-allocated copy of the answer. The caller must free it.
char *ask_question_shelf(char*question);

/// @brief Sorts an array in place with insertion sort.
/// @param array The array to sort.
/// @param length The number of elements in the array.
/// @param cmp The comparison function. Negative means a sorts before b,
///        zero means equal, positive means a sorts after b.
/// @note Stable. Runs in O(n^2) in the worst case, which is fine for the
///       array sizes list_merchandise and list_shelves produce.
void insertion_sort(elem_t *array, size_t length, comparison_func cmp);

/// @brief Hashes a string key with the 31 multiplier.
/// @param key An elem_t holding a char *.
/// @return The hash value as a size_t.
/// @note Used for both merchandise names and shelf names. Two different
///       strings can hash to the same value, which the table handles.
size_t ioopm_string_knr_hash(elem_t key);

/// @brief Hashes a size_t key.
/// @param key An elem_t holding an integer.
/// @return The key itself, since it does not need mixing.
size_t ioopm_size_t_knr_hash(elem_t key);

/// @brief Compares two string keys for equality.
/// @param str1 An elem_t holding a char *.
/// @param str2 An elem_t holding a char *.
/// @return true if the strings are identical, false otherwise.
bool string_compare(elem_t str1, elem_t str2);

/// @brief Compares two integer keys for equality.
/// @param int1 An elem_t holding an integer.
/// @param int2 An elem_t holding an integer.
/// @return true if the values are equal, false otherwise.
bool size_t_compare(elem_t int1, elem_t int2);

#endif