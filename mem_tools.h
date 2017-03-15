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
#ifndef _MEM_H_
#define _MEM_H_

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

class Vec_Reg;
class LM_Bank;
class prefetcher;

// Class for the Vector Register File
class Vec_Reg{

	// Size of each Vector Register
	int VR_size;

	// Number of Registers
	int num_VecReg;

	// The Vector Register
	unsigned int **vector_register;

public:

	// Constructor
	Vec_Reg(unsigned = 64, int = 256);

	// Destructor
	~Vec_Reg();

	// Load a stream of a given size from a given offset
	// into the Vector Register File.
	void VR_load(unsigned int reg, symbol_t *stream, int offset, int size, int *cycle);

	// Copy data of a given size from a given offset
	// into the given Prefetcher.
	void cpy_to_prefetcher(unsigned int reg, prefetcher *prefetcher, int size, int offset);

	// Return the number of Registers in the VR
	int ret_numVR();

	// Return the size of the VR
	int ret_VRsize();

	// Re-allocate the vector register itself with a new (given) size
	void re_vecreg(int new_size);

};

inline int Vec_Reg::ret_numVR() {return num_VecReg;}
inline int Vec_Reg::ret_VRsize() {return VR_size;}

//=========================================================================================//
//=========================================================================================//

// Class for the LM Bank
class LM_Bank{
	friend class sequence_logic;
	friend class lane;

	// The associated automaton with this Bank--only one of these will be initialized
	automata *automaton;

	// The type of automaton associated with this Bank
	unsigned int type;

public:

	// Constructor
	LM_Bank(unsigned int auto_type);

	// Destructor
	~LM_Bank();

	// Load the automata into the bank
	void load_automaton(char *file_name, unsigned int auto_type);

	// Function to retrieve the automata in this bank
	automata *get_automata();

	// Function to return the automata type
	unsigned int get_type();

	// Examine a given state with a given symbol
	void examine(state_t state, symbol_t symbol, int *num_match, int *cycle, int *match_buckets);

};

inline automata *LM_Bank::get_automata(){return automaton;}
inline unsigned int LM_Bank::get_type(){return type;}

//=========================================================================================//
//=========================================================================================//

// Class for the prefetcher for each UAP lane
class prefetcher{

	// Allows the VRF to access private variables of the
	// prefetcher class
	friend class Vec_Reg;
	friend class lane;

	// Size of the prefetcher
	int size;

	// The prefetcher's buffer
	symbol_t *buffer;

	// Current Vec_Reg offset
	int VR_offset;

	// The prefetcher's cycle counter
	int cycle;

public:

	// Constructor
	prefetcher(unsigned = 1024);

	// Destructor
	~prefetcher();

	// Load data into the prefetcher from the VRF 
	void prefetch(Vec_Reg *VRF, int dst_VR, int size, int offset);
};

#endif /*_MEM_H_*/