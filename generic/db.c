#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

struct item {
    char *name;
    char *desc;
    int price;
    char *shelf;
};

typedef struct item item_t;

float price_to_kr(int price) {
    return (float)price / 100; // float cast used to prevent rounded numbers
}


void print_item(item_t *item) {
    // %.2f -> its a float and use 2 digits after float-point
    printf("Name:  %s\nDesc:  %s\nPrice: %.2f\nShelf: %s\n", item->name,
                                                             item->desc,
                                                             price_to_kr(item->price),
                                                             item->shelf);
}

void list_db(item_t *items, int no_items) {

    for (int counter = 0; counter < no_items; counter++) {
        printf("%d. %s\n", counter + 1, items[counter].name);
    }
}

void print_all_items(item_t *db, int db_siz) {
    if (db_siz == 0) {
        printf("Finns ingen vara att visa");
    } else {
        for (int i = 0; i < db_siz; ++i)
        {
            print_item(&db[i]);
            printf("\n");
        }
    }
}

char *print_menu(void) {
    // Added option [d], felt like it was missing
    char *menu = ("[L]ägga till en vara\n"
                  "[T]a bort en vara\n"
                  "[R]edigera en vara\n"
                  "Ån[g]ra senaste ändringen\n"
                  "Lista [h]ela varukatalogen\n"
                  "Lista hela varukatalogen, med all befintlig [d]ata för varorna\n"
                  "[A]vsluta\n");
    
    return menu;
}


item_t make_item(char *name, char *desc, int price, char *shelf) {
    item_t new_item = { 
        .name = name,
        .desc = desc,
        .price = price,
        .shelf = shelf
    };

    return new_item;
}

item_t input_item(void) {
    char *name_of_item = ask_question_string("Name of new item: ");
    char *desc_of_item = ask_question_string("Description of new item: ");
    int price_of_item = ask_question_int("Price of item: ");
    char *shelf_of_item = ask_question_shelf("Shelf for item: ");

    item_t new_item = make_item(name_of_item, desc_of_item, price_of_item, shelf_of_item);

    return new_item;
}

char *magick(char *arr_1[], char *arr_2[], char *arr_3[], int arr_len) {
    int buf_size = 255;
    char buf[buf_size];
    int index = 0;

    // random indexes later used to access strings in arrays
    int i_1 = rand() % arr_len;
    int i_2 = rand() % arr_len;
    int i_3 = rand() % arr_len;

    strcpy(&buf[index], arr_1[i_1]); // Copy-paste chars in memory. Copies string of right argument and pastes at left argument.
    index += strlen(arr_1[i_1]); // increment index to find next 'free' slot in buf.
    buf[index] = '-';
    index++;

    strcpy(&buf[index], arr_2[i_2]);
    index += strlen(arr_2[i_2]);
    buf[index] = ' ';
    index++;

    strcpy(&buf[index], arr_3[i_3]);
    index += strlen(arr_3[i_3]);
    buf[index] = '\0';

    return strdup(buf);
}


bool is_valid_menu_choice(char *str) {
    int str_len = strlen(str);

    //check if valid str
    if (str_len != 1) {
        return false;
    }
    
    bool valid = strchr("LlTtRrGgHhDdAa", str[0]) != NULL; // checks if str[0] is the same as something in left argument.

    return valid;
}

// Needed because toupper() takes an int as arg but a convert_func expects a char * as arg
answer_t convert_menu(char *str) {
    answer_t answer;
    answer.int_value = toupper(str[0]);

    return answer;
}

char ask_question_menu(char *question) {
    return ask_question(question, is_valid_menu_choice, convert_menu).int_value;
}


void edit_db (item_t *items, int db_siz) {
    int item_of_intrest;

    do {
        item_of_intrest = ask_question_int("Vara att editera: ");
        
    } while (item_of_intrest < 1  || item_of_intrest > db_siz);

    int valid_index = item_of_intrest - 1;
    printf("\n    VALD VARA:\n");
    print_item(&items[valid_index]);
    items[valid_index] = input_item();
}

void remove_item_from_db(item_t *items, int *db_siz) {
    // check if items is empty
    if (*db_siz == 0) {
        printf("Finns ingen vara att ta bort!\n");
    } else {
        int item_of_intrest;
    
        do {
            item_of_intrest = ask_question_int("Vara att radera: ");
            
        } while (item_of_intrest < 1  || item_of_intrest > *db_siz);
    
        int valid_index = item_of_intrest - 1;
    
        printf("\n    DENNA VARA RADERAS:\n");
        print_item(&items[valid_index]);
    
        // traverse items from item_of_intrest to end,
        // move everything one step to the left effectivly erasing item_of_intrest while keeping indices correct
        for (; valid_index < *db_siz - 1; valid_index++) {
            items[valid_index] = items[valid_index + 1];
        }
        (*db_siz)--; // update size of db
    }
}

void add_item_to_db(item_t *items, int *db_siz) {
    if (*db_siz == 16) {
        printf("Varukatalog full (16/16), kan inte lägga till fler varor!\n");
    } else {
        item_t new_item = input_item(); //create new item
    
        items[*db_siz] = new_item; // place it in memory
        (*db_siz)++; //increment size of database

        printf("\n    TILLAGD VARA:\n");
        print_item(&new_item);
    }
}


int event_loop(item_t *db, int *db_siz) {
    bool run = true;
    char user_choice;

    while (run) {
        user_choice = ask_question_menu(print_menu());
        if (user_choice == 'L') {
            add_item_to_db(db, db_siz);
        } else if (user_choice == 'T') {
            remove_item_from_db(db, db_siz);
        } else if (user_choice == 'R') {
            edit_db(db, *db_siz);
        } else if (user_choice == 'G') {
            printf("Not yet implemented!\n");
        } else if (user_choice == 'H') {
            list_db(db, *db_siz);
        } else if (user_choice == 'D') {
            print_all_items(db, *db_siz);
        } else if (user_choice == 'A') {
            run = false;
        }
        
    }
    return 1;
}


int main(int argc, char *argv[])
{
    char *array1[] = { "Laser",        "Polka",    "Extra" };
    char *array2[] = { "förnicklad",   "smakande", "ordinär" };
    char *array3[] = { "skruvdragare", "kola",     "uppgift" };

  if (argc < 2)
  {
    printf("Usage: %s number\n", argv[0]);
  }
  else
  {
    item_t db[16]; // Array med plats för 16 varor
    int db_siz = 0; // Antalet varor i arrayen just nu

    int items = atoi(argv[1]); // Antalet varor som skall skapas

    if (items > 0 && items <= 16)
    {
      for (int i = 0; i < items; ++i)
      {
        // Läs in en vara, lägg till den i arrayen, öka storleksräknaren
        item_t item = input_item();
        db[db_siz] = item;
        ++db_siz;
      }
    }
    else
    {
      puts("Sorry, must have [1-16] items in database.");
      return 1; // Avslutar programmet!
    }

    for (int i = db_siz; i < 16; ++i)
      {
        char *name = magick(array1, array2, array3, 3);
        char *desc = magick(array1, array2, array3, 3);
        int price = random() % 200000;
        char shelf[] = { random() % ('Z'-'A') + 'A',
                         random() % 10 + '0',
                         random() % 10 + '0',
                         '\0' };
        item_t item = make_item(name, desc, price, strdup(shelf));

        db[db_siz] = item;
        ++db_siz;
      }

     // Skriv ut innehållet
     /*
     for (int i = 0; i < db_siz; ++i)
     {
       print_item(&db[i]);
       printf("\n");
     }
     */
    event_loop(db, &db_siz);
  }
  return 0;
}