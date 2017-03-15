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

#ifndef _DMA_H_
#define _DMA_H_

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
#include "mem_tools.h"
#include "metric.h"

class Vec_Reg;
class metric_manager;

class DMA{
	friend class PMA;

	// DMA cycle counter
	int gather_cycle;

	// Vector Register cycle counter
	int VecReg_cycle;

	// The DMA buffer
	symbol_t **DMA_buffer;

	// Stream Length
	unsigned int stream_len;

	// The number of streams
	unsigned int num_stream;

	// The Metric Manager
	metric_manager *MM;
	
	// The Vector Register File
	Vec_Reg *Vector_Register;

public:

	// Constructor
	DMA(unsigned int verb = 0, unsigned = 256, unsigned = 0, Vec_Reg *VRF = NULL);

	// Destructor
	~DMA();

	// Function to read the input streams from a given file
	void read_streams(char *file_name);

	// Function to read streams into the Vector Register
	void send_streams(Vec_Reg *Vector_Register, int *VecReg_cycle);

	// Load the Vector Register
	void VR_load(char *input_filename, int *gather_cycle, int *VecReg_cycle, int num_stream, Vec_Reg *Vector_Register);

	// Start the Gather Stage!
	void gather(char *input_filename);
};

#endif /*_DMA_H_*/