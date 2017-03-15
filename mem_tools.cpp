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

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mem_tools.h"

using namespace std;

//================================================================//
//=====================Vec_Reg File Functions=====================//
//================================================================//

Vec_Reg::Vec_Reg(unsigned int N, int step)
{
	// Simple initialization
	VR_size = step;
	num_VecReg = N;

	// Make the actual Vector Register
	vector_register = (unsigned int **) allocate_array(N, sizeof(unsigned int*));
	for(int index = 0; index < num_VecReg; index++)
		vector_register[index] = (unsigned int *) allocate_array(VR_size, sizeof(unsigned int));
	for(int i = 0; i < num_VecReg; i++)
		for(int j = 0; j < VR_size; j++)
			vector_register[i][j] = 0;

	return;
}

Vec_Reg::~Vec_Reg()
{
// EXTERMINATE! EXTERMINATE! EXTERMINATE!
/*
       			  ___                       ___ 
          D>=G==='   '.				D>=G==='   '.
                |======|				  |======|
                |======|				  |======|
            )--/]IIIIII]			  )--/]IIIIII]
                |______|				  |______|
               C O O O D 				 C O O O D
              C O  O  O D         		C O  O  O D
             C  O  O  O  D 			   C  O  O  O  D
             C__O__O__O__D 			   C__O__O__O__D
            [_____________]			  [_____________]
*/
	for(int index = 0; index < num_VecReg; index++)
		free(vector_register[index]);
	free(vector_register);

	return;
}

void Vec_Reg::VR_load(unsigned int reg, symbol_t *stream, int offset, int size, int *cycle)
{
	// Error Checking
	if(size > VR_size)
		error("Vec_Reg::VR_load: Load size exceeds max VR_size");
	if(size <= 0)
		error("Vec_Reg::VR_load: Invalid load size");

	// Reset the register
	for(int i = 0; i < VR_size; i++){
		vector_register[reg][i] = 0;
	}

	// Load all characters into the VRF
	for(int j = 0; j < size; j++){
		vector_register[reg][j] = (unsigned int) stream[j + offset];
	}

	// Advance the cycle counter.
	(*cycle)++;

	return;
}

void Vec_Reg::cpy_to_prefetcher(unsigned int reg, prefetcher *prf, int size, int offset)
{
	// Error Checking
	if(size <= 0)
		error("Vec_Reg::cpy_to_prefetcher: Invalid copy size");
	if(size > VR_size)
		error("Vec_Reg::cpy_to_prefetcher: Size exceeds VRF size");
	if(size > prf->size)
		error("Vec_Reg::cpy_to_prefetcher: Size exceeds prefetcher size");

	// Copy to the data to the prefetcher
	for(int i = 0; i < size; i++){
		if((i + offset) < VR_size)
			prf->buffer[i] = (symbol_t) vector_register[reg][i + offset];
		else{
			prf->buffer[i] = 0;
		}
	}
	return;
}

void Vec_Reg::re_vecreg(int new_size)
{
	// Get rid of the old vector register. Nobody liked that one anyway.
	for(int index = 0; index < num_VecReg; index++)
		free(vector_register[index]);
	free(vector_register);

	// Set the new size.
	num_VecReg = new_size;

	// Allocate and initialize the new and shiny Vector Register.
	unsigned int **new_vector_register = (unsigned int **) allocate_array(new_size, sizeof(unsigned int*));
	for(int index = 0; index < new_size; index++)
		new_vector_register[index] = (unsigned int *) allocate_array(VR_size, sizeof(unsigned int));
	for(int i = 0; i < new_size; i++){
		for(int j = 0; j < VR_size; j++){
			new_vector_register[i][j] = 0;
		}
	}

	// Set the new (and shiny) Vector Register as the one we'll use.
	vector_register = new_vector_register;

	return;
}

//================================================================//
//=======================LM Bank Functions========================//
//================================================================//

LM_Bank::LM_Bank(unsigned int auto_type)
{
	type = auto_type;
	// Will be filled in during load_automaton
	automaton = NULL;

	return;
}


LM_Bank::~LM_Bank()
{
	// This causes problems...will update soon?
	//delete automaton;

	return;
}

void LM_Bank::load_automaton(char *filename, unsigned int auto_type)
{
	// Initialize!
	automata *autom;

	// Initialize all of the automata, switching on the type of automaton to be created
	// Each case has its own array of automata to be passed to the DMA later
	switch(auto_type){
		// NFA type
		case 1: 
		{
			fprintf(stderr,"\nImporting from file %s ...\n\n", filename);

			// Opening the automaton file
			FILE *file_name = fopen(filename, "r");

			// Sanity Check
			if(file_name == NULL)
				error("LM_Bank::load_automaton: Could not open automata");

			// Initialization of the automata
			NFA *new_automaton = new NFA();

			// Set the automata* to match the newly allocated NFA class
			autom = new_automaton;

			// Read the automaton
			autom->read_auto(file_name);

			// Initialize some of the parts of the automata* class
			autom->num_states = new_automaton->ret_size();
			autom->num_transitions =  new_automaton->ret_num_transitions();
			autom->base_transitions = new_automaton->ret_state_table();

			// Close the automaton file
			fclose(file_name);
			
			// Set the LM_Bank's automaton as the automaton just read
			automaton = autom;

			break;
		}
		// DFA type
		case 2: 
		{
			fprintf(stderr,"\nImporting from file %s ...\n\n", filename);

			// Opening the file
			FILE *file_name = fopen(filename, "r");

			// Sanity Check
			if(file_name == NULL)
				error("LM_Bank::load_automaton: Could not open automata");

			// Initialization of the automata
			DFA *new_automaton = new DFA();

			// Set the automata* to match the newly allocated DFA class
			autom = new_automaton;

			// Read the automaton
			autom->read_auto(file_name);

			// Initialize some of the parts of the automata* class
			autom->num_states = new_automaton->ret_size();
			autom->num_transitions =  new_automaton->ret_num_transitions();
			autom->base_transitions = new_automaton->get_state_table();

			// Close the file
			fclose(file_name);

			// Set the LM_Bank's automaton as the automaton just read
			automaton = autom;
			
			break;
		}
		// A_DFA type
		case 3: 
		{
			fprintf(stderr,"\nImporting from file %s ...\n\n", filename);

			// Opening the file
			FILE *file_name = fopen(filename, "r");

			// Sanity Check
			if(file_name == NULL)
				error("LM_Bank::load_automaton: Could not open automata");
			
			// Initialization of the automata
			A_DFA *new_automaton = new A_DFA();

			// Set the automata* to match the newly allocated A_DFA class
			autom = new_automaton;

			// Read the automaton
			autom->read_auto(file_name);

			// Initialize some of the parts of the automata* class
			autom->num_states = new_automaton->ret_size();
			autom->num_transitions =  new_automaton->ret_num_transitions();
			autom->base_transitions = new_automaton->get_state_table();

			// Close the file
			fclose(file_name);

			// Set the LM_Bank's automaton as the automaton just read
			automaton = autom;
			
			break;
		}
		default: error("LM_Bank::load_automaton: Unknown automata type");
				break;
	}
	return;
}

void LM_Bank::examine(state_t state, symbol_t symbol, int *num_match, int *cycle, int *match_buckets)
{
	// Call the automaton's transition function over the given symbol in the given state
	automaton->do_transition(symbol, state, num_match, cycle, match_buckets);

	return;
}

//================================================================//
//======================Prefetcher Functions======================//
//================================================================//

prefetcher::prefetcher(unsigned int pre_size)
{
	// Simple initializations
	size = pre_size;
	VR_offset = 0;
	cycle = 0;

	// Allocate the prefetcher's buffer.
	buffer = (symbol_t *) allocate_array(size, sizeof(unsigned char));
	for(int i = 0; i < size; i++)
		buffer[i] = 0;

	return;
}

prefetcher::~prefetcher()
{
	// Free the buffer.
	free(buffer);
	return;
}

void prefetcher::prefetch(Vec_Reg *VRF, int dst_VR, int size, int offset)
{
	// Access the VRF, and call the correct Vector Register's
	// copy_to_prefetcher function
	VRF->cpy_to_prefetcher(dst_VR, this, size, offset);

	// Advance the cycle counter
	cycle++;

	return;
}