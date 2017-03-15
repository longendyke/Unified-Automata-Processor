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
/*
 * Copyright (c) 2007 Michela Becchi and Washington University in St. Louis.
 * All rights reserved
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. The name of the author or Washington University may not be used
 *       to endorse or promote products derived from this source code
 *       without specific prior written permission.
 *    4. Conditions of any other entities that contributed to this are also
 *       met. If a copyright notice is present from another entity, it must
 *       be maintained in redistributions of the source code.
 *
 * THIS INTELLECTUAL PROPERTY (WHICH MAY INCLUDE BUT IS NOT LIMITED TO SOFTWARE,
 * FIRMWARE, VHDL, etc) IS PROVIDED BY  THE AUTHOR AND WASHINGTON UNIVERSITY
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR WASHINGTON UNIVERSITY
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS INTELLECTUAL PROPERTY, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * */

/*
 * File:   linked_set.c
 * Author: Michela Becchi
 * Email:  mbecchi@cse.wustl.edu
 * Organization: Applied Research Laboratory
 * 
 */

#include "utils.h"

void error(char *message)
{
    fprintf(stderr, "ERROR: %s\n", message);
    exit(1);
}

void message(char *message)
{
    fprintf(stdout, "MESSAGE: %s\n", message);
    fflush(stdout);
    return;
}

void *allocate_array(int size, size_t data_size)
{
    void *array = malloc(size * data_size);
    if(array == NULL){
        error("allocate_array: could not allocate memory.");
    }
    return array;
}

void *reallocate_array(void *array, int size, size_t data_size)
{
    void *new_array = realloc(array, size * data_size);
    if(new_array == NULL){
        error("reallocate_array: could not allocate memory.");
    }
    return new_array;
}

/* LINKED LIST FUNCTIONS */

//Constructor with default set as the empty set
linked_list::linked_list(item_t new_data)
{
    data = new_data;
    next = NULL;
}

//Destructor (recursive)
linked_list::~linked_list()
{
    if(next!=NULL) 
        delete next;
}

//Is the list empty?
bool linked_list::empty()
{
    return (data == INVALID);
}

//Are two lists identical? (recursive)
bool linked_list::equal(linked_list *list)
{
    if (list == NULL && !empty())
        return false;
    if (data != list->data)
        return false;
    if (next == NULL && list->next == NULL)
        return true;
    if (next == NULL || list->next == NULL)
        return false;
    return next->equal(list->next);
}

//Insert an item into a linked list
void linked_list::insert(item_t insert)
{
    if(data == 0){
        data = insert;
        return;
    }else if (insert < data){
        linked_list *new_list = new linked_list(data);
        data = insert;
        new_list->next = next;
        next = new_list;
        return;
    }else if(insert == data){
        return;
    }else{
        if(next == NULL){
            next = new linked_list(insert);
            return;
        }else if (insert < next->data){
            linked_list *new_list = new linked_list(insert);
            new_list->next = next;
            next = new_list;
            return;
        }else{
            next->insert(insert);
            return;
        }
    }
}

//Add elements of a list to the current list
void linked_list::add_list(linked_list *add)
{
    linked_list *temp = add;
    while(temp != NULL && temp->data != INVALID){
        insert(temp->data);
        temp = temp->next;
    }
    return;
}

//Delete all elements from a linked list
void linked_list::flush()
{
    if (next != NULL) 
        delete next;
    next = NULL;
    data = INVALID;
}

//Search function
bool linked_list::search(item_t find)
{
    if(data == find)
        return true;
    if(next == NULL)
        return false;
    return next->search(find);
}

//Return the size of a linked list
unsigned linked_list::size()
{
    if(data == INVALID)
        return 0;
    if(next == NULL)
        return 1;
    return 1 + next->size();
}