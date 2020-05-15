/**
*  Name: Porfirio Mohabir
 * @file
 * See specification here: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html
 */


#include <fcntl.h>
#include <locale.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <math.h>

#include "history.h"
#include "debug.h"

struct history_entry arr[HIST_MAX];
struct history_entry arr_copy[HIST_MAX];
int end_pointer = 0;
int entry_count = 0; 

/** swap prototypes */
void swap(struct history_entry *a, struct history_entry *b);
/** copy_array prototypes */
void copy_array();

/** Function Adds command and its ID to History Array in a circular Array
 * Every Command is added except commands that starts with "!";
 */
void add_entry(char *l, int count){

	if(!start("!", l)){
		char *temp = strdup(l);


		if(end_pointer != HIST_MAX){
			end_pointer++; 
		}
		int index = entry_count % HIST_MAX;
		arr[index].cmd_line = temp;
		arr[index].cmd_id = count;
		entry_count++;
	}

}

/**
 *Fucntion frees history array
 *Iterate through array and free every struct
 */
void free_history(){
	for(int i = 0; i<end_pointer; i++){
		free(arr[i].cmd_line);
	}
}

/** 
 * Swaps two different structs is an array of Structs
 */
void swap(struct history_entry *a, struct history_entry *b) { 
    struct history_entry temp = *a; 
    *a = *b; 
    *b = temp; 
} 

/** 
 * Function creates a copy of history array
 * The copy array is also global
 */
void copy_array(){
	for(int i = 0; i<end_pointer; i++){
		char *temp = strdup(arr[i].cmd_line);
		arr_copy[i].cmd_line = temp;
		arr_copy[i].cmd_id = arr[i].cmd_id;
	}
}

/**
 * Performs Bubble sort by command ID of structs
 */
void sort(){ 
	copy_array();
	int i;
 	int j; 

	for (i = 0; i < end_pointer-1; i++)         
		for (j = 0; j < end_pointer-i-1; j++)  
			if (arr_copy[j].cmd_id > arr_copy[j+1].cmd_id) 
				swap(&arr_copy[j], &arr_copy[j+1]);
	LOGP("Sorted History\n");
} 
    

/** 
 * Prints struct in linear order
 */
void print_history()
{
    /* This function should print history entries */
	sort();
	for(int i = 0; i<end_pointer;i++){
		printf("%d  %s",arr_copy[i].cmd_id, arr_copy[i].cmd_line);
	}
    LOGP("Printed History\n");
}

/**
 * Fucntion first sorted copy of the array
 * then searches the most recent command in the copy array
 * once found the search will stop and return the command line
 */
char *bang_bang_cmd(){
	sort();
	LOG("END: %d", end_pointer);
	return arr_copy[end_pointer-1].cmd_line;
}

/** 
 * The is function supprts the Single ! commands
 * Two different searches can be performed. 
 * after sorting is the @new_cmd is a number
 * search from the start and return command line
 * if @new_commmand is a command, search from the back, once found
 * search will break and return command line. 
 */
char *search(char *new_cmd, bool flag){ 
	sort();
	int i = 0; 
	if(flag){
		int cmdid = atoi(new_cmd);
		for(i = end_pointer-1; i >= 0; i--){
			if(cmdid == arr_copy[i].cmd_id){
				return arr_copy[i].cmd_line;
			}
		
		 }
	}
	else{
		for(i = end_pointer-1; i>=0; i--){
			if(start(new_cmd, arr_copy[i].cmd_line)){
				LOG("Command: %s, Entry: %s", new_cmd, arr_copy[i].cmd_line);
				return arr_copy[i].cmd_line;
			}
		}
	}


	return NULL;
}
