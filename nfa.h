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
 * File:   nfa.c
 * Author: Michela Becchi
 * Email:  mbecchi@cse.wustl.edu
 * Organization: Applied Research Laboratory
 */

#ifndef __NFA_H_
#define __NFA_H_

#include <map>
#include <set>
#include <list>

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "auto.h"

using namespace std;

class NFA;

// typedef set<state_t> st_t; // A set of states
typedef pair<symbol_t, state_t> tx_t;	//A transition for a certain symbol to a set of states
typedef list<tx_t> tx_list;	//Set of labeled transitions

// Iterator on list of transitions
#define FOREACH_TXLIST(txlist, it) \
	for(tx_list::iterator it=txlist->begin();it!=txlist->end();++it)

#define NO_STATE 0xFFFFFFFF

class NFA : public automata{

	friend class PMA;

	// Size of the NFA
	int size;

	// State Table
	// Format: table[old state][symbol] = [next_state(s)]
	state_t ***state_table;

	// Labeled Transitions
	tx_list **labeled_transitions;

	// Accepting States
	linked_list **accepted_rules;

private:

	// Number of entries allocated (for dynamic allocation)
    unsigned int entry_allocated;

public:

	// Constructor
	NFA(unsigned = 50);
	
	// De-allocator
	~NFA();
	
	// Returns the next_state from state on symbol c
	state_t *get_next_state(state_t state, symbol_t c);

	// Computes next state using default and labeled transitions
	state_t *lookup(state_t s, symbol_t c, unsigned int size);

	// Returns the (possibly empty) list of accepted rules on a state
	linked_list *accepts(state_t state);

	// Reads the NFA from file
	void read_auto(FILE *file);

	// Dumps the NFA
	void dump(FILE *log=stdout);

	// Processes a transition
	void do_transition(symbol_t c, state_t s, int *match_num, int *cycle, int *buckets);

	// Retrieve all the next states for a given transition
	state_t *retrieve_all_next(state_t *current_states, symbol_t c);

	// Series of functions to return private values of the NFA
	int ret_size();
	int ret_num_transitions();
	state_t **ret_state_table();
};

inline linked_list *NFA::accepts(state_t state) { return accepted_rules[state]; }

inline int NFA::ret_size() { return size; }

inline state_t **NFA::ret_state_table() { return *state_table; }

#endif /*__NFA_H_*/
