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

#ifndef _LAN_H_
#define _LAN_H_

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "nfa.h"
#include "dfa.h"
#include "a_dfa.h"
#include "mem_tools.h"
#include "UAP.h"

class CBQ;
class sequence_logic;
class Vec_Reg;
class LM_Bank;

class sequence_logic{
	friend class lane;
	friend class CBQ;

	// The sequence logic's cycle count
	int cycle;

	// The number of matches
	int match_num;

	// An array of buckets to keep track of which rules are found
	int *matches;

public:

	// Constructor
	sequence_logic(int num_states);

	// Destructor
	~sequence_logic();

	// Function to send bytes to the CBQ in order to retrieve the next state
	void examine_bytes(symbol_t *prf_buf, int num_bytes, CBQ *cbq, LM_Bank memory);
};


class CBQ{
	friend class sequence_logic;
	friend class lane;

	// The size of each queue
	int q_size;

	// The two queues
	state_t *q_in;
	state_t *q_out;

public:

	// Constructor
	CBQ(int new_q_size, unsigned int start_state);

	// Destructor
	~CBQ();

	// Function to retrieve the next state
	int get_next_state();

	// Function to queue next states
	void put_states(state_t *states, int num_states);

	// Switch the queues
	void switch_qin_qout();
};

#endif /*_LAN_H_*/