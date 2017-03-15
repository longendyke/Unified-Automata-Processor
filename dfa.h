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


#ifndef __DFA_H               
#define __DFA_H

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

typedef pair<symbol_t, state_t> tx_t;
typedef list<tx_t> tx_list;

// Iterator on list of transitions
#define FOREACH_TXLIST(txlist,it) \
	for(tx_list::iterator it=txlist->begin();it!=txlist->end();++it)

#define NO_STATE 0xFFFFFFFF

class DFA : public automata{

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

    int *tester;

private:

	// Number of entries allocated (for dynamic allocation)
    unsigned int entry_allocated;
	
public:	

	// Instantiates a new DFA 
	DFA(unsigned=50);
	
	// Deallocates the DFA
	~DFA();
	
	// Seturns the number of states in the DFA 
	unsigned int ret_size();

	// Sets the number of states in the DFA
	void set_size(unsigned int n);

	// Returns the next_state from state on symbol c
	state_t *get_next_state(state_t state, symbol_t c);
	
	// Returns the (possibly empty) list of accepted rules on a state
	linked_list *accepts(state_t state);
	
	// Returns the transition table
	state_t **get_state_table();
	
	// Returns the array of acceptstate pointers
	linked_list **get_accepted_rules();
	
	// The given state become the dead state
	void set_dead_state(state_t state);
	
	// Dumps the DFA
	void dump(FILE *log=stdout);
	
	// Reads the DFA from file
	void read_auto(FILE *file);
	
	// Computes next state using default and labeled transitions
	state_t lookup(state_t s, symbol_t c);

	// Processes a transition
	void do_transition(symbol_t c, state_t s, int *match_num, int *cycle, int *buckets);

	state_t *retrieve_all_next(state_t *current_states, symbol_t c);

	int ret_num_transitions();
};

inline linked_list *DFA::accepts(state_t state) { return accepted_rules[state]; }

inline unsigned int DFA::ret_size(){ return size;}

inline void DFA::set_size(unsigned int n){ size = n;}

inline state_t **DFA::get_state_table(){return state_table;}

inline linked_list **DFA::get_accepted_rules(){return accepted_rules;}

inline void DFA::set_dead_state(state_t state){dead_state=state;}

inline state_t *DFA::retrieve_all_next(state_t *current_states, symbol_t c){return get_next_state(*current_states, (symbol_t)c);}

#endif /*__DFA_H*/
