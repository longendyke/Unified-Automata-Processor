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

#ifndef _UAP_H_
#define _UAP_H_

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
#include "DMA.h"
#include "PMA.h"
#include "auto.h"
#include "lane_tools.h"
#include "metric.h"

class UAP;
class lane;
class LM_Bank;
class Vec_Reg;
class prefetcher;
class DMA;
class PMA;
class CBQ;
class sequence_logic;

class UAP{
	friend class PMA;

	// Number of UAP lanes
	int lane_num;

	// Number of UAP memory banks
	int num_mem;

	// The UAP lanes
	lane *lanes;

	// The memory bank
	LM_Bank *memory;

	// The Vector Register File (VRF)
	Vec_Reg *VRF;

	// The step size
	int UAP_step;

	// A pointer to the metric manager
	metric_manager *MM;

public:

	// Constructor
	UAP(int = 64, int = 64, int = 256);

	// Destructor
	~UAP();

	// Function to load all the automata into their respective lanes
	void load_automata(char *base_file_prefix, int file_num, int auto_type);

	// Function to configure the UAP
	void UAP_config(int init_state, unsigned long long int lane_mask, int vec_sharing);

	// Function to begin traversing an automaton
	void traverse(int start, int step);

	// Function to read values/configurations in from a config file
	void read_config_file(char *filename);
};

class lane{

	// The sequence logic of the lane
	sequence_logic *seq;

	// The CBQ of the lane
	CBQ *cbq;

	// The lane's prefetcher
	prefetcher *prf;

	// The lane's destination in the VRF
	int vec_sharing;

	// Is this lane masked?
	bool mask;

	// The lane's initial state
	int init_state;

	// Pointer to the metric manager
	metric_manager *MM;

public:

	// Constructor
	lane(unsigned = 0, bool = false);

	// Destructor
	~lane();

	// Function to configure the lane
	void lane_config(prefetcher *the_prf, CBQ *the_cbq, sequence_logic *the_seq, int nvec_sharing, bool mask, int init_state, metric_manager *MM);

	// Function to begin traversing an automaton
	void traverse(int start, int step, LM_Bank memory, Vec_Reg *VRF);

	// Set the initial state of a lane
	void set_Istate(int init);

	// Set the destination VRF for a lane
	void set_vec_sharing(int dst);
};

inline void lane::set_Istate(int init) {init_state = init; return;}
inline void lane::set_vec_sharing(int dst) {vec_sharing = dst; return;}


#endif /*_UAP_H_*/