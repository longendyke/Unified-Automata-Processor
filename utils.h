/* 
================================================================
    Summer 2015 Software Research Assistant Position
    The University of Chicago Department of Computer Science
    The Unified Automata Processor        
    Kevin Fang and Willem Longendyke
    9/17/2015
----------------------------------------------------------------
++ This work is licensed under the Creative Commons 		  ++
++ Attribution-NonCommercial-ShareAlike 4.0 International 	  ++
++ License. To view a copy of this license, visit 			  ++
++ http://creativecommons.org/licenses/by-nc-sa/4.0/.		  ++
================================================================
*/

#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Type definitions for states (unsigned integers), symbols (characters), and items (unsigned ints)
typedef unsigned int state_t;
typedef unsigned char symbol_t;
typedef unsigned int item_t;

/*  Our automata run over regular expressions which use the typical ASCII
    characters, so the size is simply 256 */
#define ALPHABET_SIZE 256

// Definition of the empty set
#define INVALID 0xFFFFFFFF

/* FUNCTIONS */

//Error message
void error(char *message);

//System message
void message(char *message);

// max and min 
inline int max(int x, int y) { return x > y ? x : y; }
inline double max(double x, double y) { return x > y ? x : y; }
inline int min(int x, int y) { return x < y ? x : y; }
inline double min(double x, double y) { return x < y ? x : y; }

//Memory allocation

//Allocate a generic array (with error checking)
void *allocate_array(int size, size_t data_size);

//Reallocate a generic array (with error checking)
void *reallocate_array(void *array, int size, size_t data_size);

//Easy-use functions for different data types

#define allocate_char_array(size) (char *) allocate_array( size, sizeof( char ) )
#define allocate_string_array(size) (char **) allocate_array( size, sizeof( char *) )
#define allocate_int_array(size) (int *) allocate_array( size, sizeof( int ) )
#define allocate_uint_array(size) (unsigned int *) allocate_array( size, sizeof(unsigned int ) )
#define allocate_bool_array(size) (bool *) allocate_array( size, sizeof( bool ) )
#define allocate_state_array(size) (state_t *) allocate_array( size, sizeof( state_t ) )
#define allocate_state_matrix(size) (state_t **) allocate_array( size, sizeof( state_t *) )
#define allocate_nfa_state_matrix(size) (state_t ***) allocate_array(size, sizeof(state_t **))

#define reallocate_char_array(size) (char *) reallocate_array(array, size, sizeof( char ) )
#define reallocate_int_array(array,size) (int *) reallocate_array(array, size, sizeof( int ) )
#define reallocate_bool_array(array,size) (bool *) reallocate_array( array,size, sizeof( bool ) )
#define reallocate_uint_array(array,size) (unsigned int *) reallocate_array(array, size, sizeof(unsigned int ) )
#define reallocate_state_array(array,size) (state_t *) reallocate_array(array, size, sizeof( state_t ) )
#define reallocate_state_matrix(array,size) (state_t **) reallocate_array(array, size, sizeof( state_t *) )
#define reallocate_string_array(array,size) (char **) reallocate_array(array, size, sizeof( char *) )

/*=========*/
/* CLASSES */
/*=========*/

/* START: LINKED LIST */
class linked_list{

    //Value
    item_t data;

    //Next pointer
    linked_list *next;

public:

    //Constructor with default set as the empty set
    linked_list(item_t data = INVALID);

    //Destructor (recursive)
    ~linked_list();

    //Is the list empty?
    bool empty();

    //Are two lists identical? (recursive)
    bool equal(linked_list *list);

    //Insert an item into a linked list
    void insert(item_t insert);

    //Add elements of a list to the current list
    void add_list(linked_list *add);

    //Return the data from the front of a linked list
    item_t ret_data();

    //Return the next part of a linked list
    linked_list *ret_next();

    //Delete all elements from a linked list
    void flush();

    //Search function
    bool search(item_t find);

    //Return the size of a linked list
    unsigned size();
};

inline item_t linked_list::ret_data(){return data;}
inline linked_list *linked_list::ret_next(){return next;}
/* END: LINKED LIST */

#endif //_UTILS_H_