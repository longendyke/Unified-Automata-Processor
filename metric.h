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

#ifndef _MTM_H_
#define _MTM_H_

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
#include "auto.h"
#include "lane_tools.h"
#include "mem_tools.h"
#include "UAP.h"
#include "DMA.h"
#include "PMA.h"

class UAP;
class DMA;
class Vec_Reg;
class PMA;
class lane;
class sequence_logic;
class CBQ;
class prefetcher;

class metric_manager{
	friend class PMA;
	friend class DMA;
	friend class UAP;
	friend class Vec_Reg;
	friend class prefetcher;
	friend class sequence_logic;

	// The verbosity of the program
	unsigned int verbosity;

	// Step size
	unsigned int step;

	// Round number
	int round;

	// Which lane we're working with
	int lane_num;

	// A counter for the total cycle of the UAP
	int overall_cycle;

	// A counter for the amount of time the UAP has taken to process
	double overall_time;

	// The total number of characters read into the Vector Register (given by the DMA)
	unsigned int number_characters;

public:

	// Constructor
	metric_manager(unsigned int verb, unsigned int stp, int rnd, int lane);

	// Destructor
	~metric_manager();

	// Function to report cycles in the DMA
	void report_DMA(double elapsed_secs, int gather_cycle, int VR_cycle);

	// Function to report cycles in the prefetcher
	void report_prefetch(double elapsed_secs, int prf_cycle);

	// Function to report sequence logic cycles
	void report_sequence(double elapsed_secs, int cycle, int match_num, int *matches, int num_states);

	// Function to print out the final report
	void report_final();
};

#endif /*_MTM_H_*/