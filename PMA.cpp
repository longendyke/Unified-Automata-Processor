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

#include "PMA.h"

using namespace std;

PMA::PMA(unsigned numfile, int num_lane, int num_mem, unsigned int num_stream, unsigned int steep, unsigned int verb)
{
	// Allocate and initialize the new UAP
	uap = NULL;
	uap = new UAP(num_lane, num_mem, steep);

	// Retrieve the VRF to be given to the DMA
	Vec_Reg *for_dma = NULL;
	for_dma = uap->VRF;

	// Allocate and initialize the new DMA
	dma = NULL;
	dma = new DMA(verb, steep, num_stream, for_dma);

	// Give the UAP the pointer to the DMA
	uap->MM = dma->MM;

	// More Initialization
	base_file_prefix = NULL;
	file_num = numfile;

	return;
}

PMA::~PMA()
{
	// More needs to be added here to involve the deallocation of the UAP and the DMA
	free(base_file_prefix);

	return;
}

void PMA::PMA_run(char *configure, char *prefix, unsigned int num_auto, unsigned int auto_type, char *input, int step)
{

	// Initialize values for the PMA
	base_file_prefix = prefix;
	file_num = num_auto;
	
	// Start of the Load Stage.
	// Loads the automata into the UAP's local memory banks
	uap->load_automata(prefix, num_auto, auto_type);

	// Start of the Configure Stage.
	// Reads configuration properties from a config file
	// and properly adjusts the UAP.
	uap->read_config_file(configure);

	// Start of the Gather Stage.
	// Reads all input from a file into the VRF.
	dma->gather(input);

	// Start of the Process Stage.
	// Beginning of the traversal of the input symbols over the loaded automata.
	uap->traverse(0, step);

	return;
}