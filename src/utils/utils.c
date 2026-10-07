#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

extern char *strdup(const char *);

typedef union { 
  int   int_value;
  float float_value;
  char *string_value;
} answer_t;

typedef bool check_func(char *);

typedef answer_t convert_func(char *);


void clear_input_buffer() {
    int c;
    do {
            /*
            removes the first input in the input stream (keyboard buffer) after scanf attempted the conversion.
            say user input is '1a\n', scanf moves 1 to memory &result, '\n' is still in the input stream and through while loop, getchar() clears junk so that the keyboard buffer is empty for next I/O.
            */
        c = getchar(); 
    } while (c != '\n' && c != EOF);
}

int read_string(char *buf, int buf_siz) {
    int conversions = 0;
    int buf_index = 0;
    char scanned_char;

    do {
        scanned_char = getchar();

        if (scanned_char == '\n') {
            break;
        }

        buf[buf_index] = scanned_char;

        conversions++;
        buf_index++;
 
    } while (buf_index < buf_siz);

    if (scanned_char != '\n' || buf_index == buf_siz) {
        clear_input_buffer();
    }
    
    buf[buf_index] = '\0';

    return conversions;
}

char *trim(char *str)
{
  char *start = str;
  char *end = start + strlen(str)-1;

  while (isspace(*start)) ++start;
  while (isspace(*end)) --end;

  char *cursor = str;
  for (; start <= end; ++start, ++cursor)
    {
      *cursor = *start;
    }
  *cursor = '\0';

  return str;
}

answer_t ask_question(char *question, check_func *check, convert_func *convert) {
    int buf_size = 255;
    char buf[buf_size];

    int read;
    do {
        printf("%s", question);
        read = read_string(buf, buf_size);

        trim(buf);

        if (!check(buf)) {
            read = 0;
        }
    } while (read == 0);
    
    answer_t answer = convert(buf);

    return answer;
}


// INTS
bool is_number(char *str) {
    int str_len = strlen(str);
    int index = 0;

    //check if valid str
    if (str_len == 0) {
        return false;
    }
    
    // check if negative
    if (str[index] == '-') {
        index++;
        
        if (index == str_len) {
            return false;
        }
    }
    
    // traverse str
    for (; index < str_len; index++) {
        if (!isdigit(str[index])) {
            return false;
        }
    }

    return true;
}

int ask_question_int(char *question) {
    return ask_question(question, is_number, (convert_func *) atoi).int_value;
}


// FLOATS
answer_t make_float(char *str) {
  return (answer_t) { .float_value = atof(str) };
}

bool is_float(char *str)
{
    bool dot_found = false;
    int i = 0;

    if (str[0] == '\0')
        return false;

    if (str[0] == '-')
        i++;

    if (str[i] == '\0' || str[i] == '.')
        return false;

    for (; str[i] != '\0'; i++)
    {
        if (str[i] == '.')
        {
            if (dot_found)
                return false;

            dot_found = true;
        }
        else if (!isdigit(str[i]))
        {
            return false;
        }
    }

    return dot_found;
}

double ask_question_float(char *question) {
  return ask_question(question, is_float, make_float).float_value;
}


// STRINGS
bool not_empty(char *str) {
    return strlen(str) > 0;
}

char *ask_question_string(char *question) {
  return ask_question(question, not_empty, (convert_func *) strdup).string_value;
}

// ITEMS (for lab4 db.c)

bool is_shelf(char *str) {
    int str_len = strlen(str);
    int index = 0;

    //check if valid shelf length, at least two chars are needed (one letter and one int).
    if (str_len < 2) {
        return false;
    }
    
    // check first character is a letter
    if (!isalpha(str[index])) {
        return false;
    }
    
    // increment index by one (skip letter) and traverse str
    index++;
    for (; index < str_len; index++) {
        if (!isdigit(str[index])) {
            return false;
        }
    }

    return true;
}

char *ask_question_shelf(char*question) {
    return ask_question(question, is_shelf, (convert_func *) strdup).string_value;
}

// print funcs
void print(char *string) {

    // traverse string with pointer of string, increment pointer each iteration
    while (*string != '\0') {
        putchar(*string);
        string++;
    }
}

void println(char *string) {
    print(string);
    print("\n");
}