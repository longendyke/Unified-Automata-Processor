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

#ifndef _PMA_H_
#define _PMA_H_

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
#include "UAP.h"
#include "DMA.h"

class UAP;
class DMA;

class PMA{
	friend class UAP;
	friend class DMA;

public:
	// Base file prefix to be read for automata
	char *base_file_prefix = NULL;

	// Number of files to be read
	int file_num;

	// The DMA
	DMA *dma = NULL;

	// The UAP
	UAP *uap = NULL;

public:

	// Constructor
	PMA(unsigned = 1, int = 64, int = 64, unsigned int = 64, unsigned int = 256, unsigned int verb = 0);

	// Destructor
	~PMA();

	// Function with args to begin the entire PMA process
	// For the automata type, 1 = NFA, 2 = DFA, and 3 = A-DFA
	void PMA_run(char *configure, char *prefix, unsigned int num_auto, unsigned int auto_type, char *input, int step);
};

#endif /*_PMA_H_*/