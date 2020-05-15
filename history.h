/**
*  Name: Porfirio Mohabir
 * @file
 * See specification here: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html
 */

#include <stdbool.h>

#ifndef _HISTORY_H_
#define _HISTORY_H_

#define HIST_MAX 100

/**
 * Struct type history_entry
 * Entry has a command and ID number
 */
struct history_entry {
    int cmd_id;
    char* cmd_line;
};

/** free_history prototypes */
void free_history();
/** print_history prototypes */
void print_history(); 
/** add_entry prototype */
void add_entry(char *l, int count);
/** sort prototype */
void sort();
/** start prototype */
bool start(const char *pre, const char *str);
/** bang_bang_cmd prototype */
char *bang_bang_cmd();
/** search prototype */
char *search(char *new_cmd, bool flag);
#endif
