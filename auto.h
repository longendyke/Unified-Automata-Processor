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

#ifndef __AUTO_H               
#define __AUTO_H

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

using namespace std;

class automata{

public:

	// The number of states in the automaton
	int num_states;

	// The number of transitions
	int num_transitions;

	// Labeled transitions
	state_t **base_transitions;

public:

	// Constructor
	automata();

	// Destructor
	~automata();

	// Wrapper for the read function for each automaton
	virtual void read_auto(FILE *file) =0;

	// Wrapper for the do_transition function for each automaton
	virtual void do_transition(symbol_t c, state_t s, int *match_num, int *cycle, int *buckets) =0;

	// Wrapper for the get_next_state function for each type of automaton
	virtual state_t *get_next_state(state_t state, symbol_t c) =0;

	// Wrapper for the get_next_state function for each type of automaton
	virtual state_t *retrieve_all_next(state_t *current_states, symbol_t c) =0;

	// Return the number of states in the automata class
	int ret_num_states();

};

inline int automata::ret_num_states() {return num_states;}

#endif /*__AUTO_H*/