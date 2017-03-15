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
 * File:   dfa.c
 * Author: Michela Becchi
 * Email:  mbecchi@cse.wustl.edu
 * Organization: Applied Research Laboratory
 * 
 */


#ifndef __A_DFA_H               
#define __A_DFA_H

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

class A_DFA;

typedef pair<symbol_t, state_t> tx_t;
typedef list<tx_t> tx_list;

// Iterator on the list of transitions
#define FOREACH_TXLIST(txlist,it) \
	for(tx_list::iterator it=txlist->begin();it!=txlist->end();++it)

#define NO_STATE 0xFFFFFFFF

#define MAX_DFA_SIZE_INCREMENT 50

class A_DFA : public automata{
		friend class PMA;

		//Number of states in DFA
    	unsigned int size;

    	//State Table
    	//table[old_state][symbol] = new_state
		state_t **state_table;

		// Labeled transitions
    	tx_list **labeled_tx;

		// For each state, set of accepted rules (empty-set if the state is non-accepting)
    	linked_list **accepted_rules;
        
    	// State involving no progression
    	state_t dead_state;

		// Default transitions (one per state)
        state_t *default_tx;
             
private:

	/* number of entry allocated (for dynamic allocation) */
    unsigned int entry_allocated;
	
public:	

	// Instantiates a new A-DFA
	A_DFA(unsigned=50);
	
	// Deallocates the A-DFA
	~A_DFA();
	
	// Returns the number of states in the A-DFA
	unsigned int ret_size();

	// Sets the number of states in the A-DFA
	void set_size(unsigned int n);
	
	// Returns the next_state from state on symbol c
	state_t *get_next_state(state_t state, symbol_t c);
	
	// Returns the (possibly empty) list of accepted rules on a state
	linked_list *accepts(state_t state);
	
	// Returns the transition table
	state_t **get_state_table();
	
	// Returns the array of acceptstate pointers
	linked_list **get_accepted_rules();
	
	// Returns the array of default transitions
	state_t *get_default_tx();
	
	// Returns the labeled transitions table
	tx_list **get_labeled_tx();
	
	// The given state become the dead state
	void set_dead_state(state_t state);
	
	// Dumps the A-DFA
	void dump(FILE *log=stdout);

	// Reads the A-DFA from file
	void read_auto(FILE *file);
	
	// Computes next state using default and labeled transitions
	state_t lookup(state_t s, symbol_t c);

	// Processes a transition
	void do_transition(symbol_t c, state_t s, int *match_num, int *cycle, int *buckets);

	// Get all the next possible states.
	state_t *retrieve_all_next(state_t *current_states, symbol_t c);

	// Return the number of transitions
	int ret_num_transitions();
};


inline state_t *A_DFA::get_next_state(state_t state, symbol_t c) { return &(state_table[state][c]); }

inline linked_list *A_DFA::accepts(state_t state) { return accepted_rules[state]; }

inline unsigned int A_DFA::ret_size(){ return size;}

inline void A_DFA::set_size(unsigned int n){ size = n;}

inline state_t **A_DFA::get_state_table(){return state_table;}

inline state_t *A_DFA::get_default_tx(){return default_tx;}

inline tx_list **A_DFA::get_labeled_tx(){return labeled_tx;}

inline linked_list **A_DFA::get_accepted_rules(){return accepted_rules;}

inline void A_DFA::set_dead_state(state_t state){dead_state=state;}

inline state_t *A_DFA::retrieve_all_next(state_t *current_states, symbol_t c){return get_next_state(*current_states, (symbol_t)c);}

#endif /*__A_DFA_H*/