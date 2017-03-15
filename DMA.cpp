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

#include "DMA.h"
#include "math.h"

using namespace std;

DMA::DMA(unsigned int verb, unsigned step, unsigned num_strm, Vec_Reg *VRF)
{
	// Simple initializations
	gather_cycle = 0;
	VecReg_cycle = 0;
	stream_len = step;
	num_stream = num_strm;

	Vector_Register = VRF;
	// Will be allocated and initialized in read_streams
	DMA_buffer = NULL;

	// Actually make the metric manager
	MM = new metric_manager(verb, step, 0, 0);

	return;
}

DMA::~DMA()
{
	// Get rid of all of the DMA buffer streams and the buffer itself
	for(int i = 0; i < num_stream; i++)
		free(DMA_buffer[i]);
	free(DMA_buffer);

	return;
}

void DMA::read_streams(char *file_name)
{
	// Open the file to be read from
	FILE *file = fopen(file_name, "r");

	// Sanity Check
	if(file == NULL)
		error("DMA::read_streams: Could not open input file!");

	// Determine the file length
	long int file_length;
	fseek(file, 0, SEEK_END);
	file_length = ftell(file);
	rewind(file);

	MM->number_characters = (unsigned int) file_length;

	// Determine the number of streams to be loaded, based on the given stream length.
	double number_stream = ceil((double)file_length/stream_len);

	//Initialize the correct number of streams
	num_stream =  number_stream;

	if(num_stream > 1000)
		message("~~~WARNING~~~DMA::read_streams: num_stream is greater than 1000");

	//=================================//
	//++++++DMA_BUFFER ALLOCATION++++++//
	//=================================//

	// Allocate a new DMA buffer and set it as the DMA's new buffer
	symbol_t **new_DMA_buffer = (symbol_t **) allocate_array(num_stream, sizeof(symbol_t *));
	for(int i = 0; i < num_stream; i++)
		new_DMA_buffer[i] = NULL;
	DMA_buffer = new_DMA_buffer;

	//================================//
	//++++++READ FROM INPUT FILE++++++//
	//================================//

	// Read from the file for each stream
	for(int j = 0; j < num_stream; j++){
		// Allocate a new stream to be read into
		symbol_t *stream = (symbol_t *) malloc(stream_len * sizeof(symbol_t));
		for(int i = 0; i < stream_len; i++)
			stream[i] = 0;

		// Read from the file, with error checking
		int err_chk = fread((void *) stream, sizeof(symbol_t), stream_len, file);
		if(err_chk != stream_len && !feof(file)){
			message("DMA::read_streams: WARNING--read fewer than stream_len characters from file.");
			message("Either fread failed, or we have now reached EOF\n");
		}
		if(feof(file))
			message("DMA::read_streams: Reached EOF.");

		// Store stream into the DMA buffer
		DMA_buffer[j] = (symbol_t *)stream;

		// Advance the cycle counter
		gather_cycle++;
	}
	// Close the file
	fclose(file);

	return;
}

void DMA::send_streams(Vec_Reg *VR, int *VecReg_cycle)
{
	// Get the number of Vector Registers
	int numVR = VR->ret_numVR();

	if(numVR < num_stream){
		message("Potential Error: DMA::send_streams: The Vector Register is smaller than\nthe number of streams in the DMA. Re-allocating the VRF...");
		VR->re_vecreg(num_stream);
	} else {
		message("DMA::send_streams: The Vector Register is larger than\nthe number of streams in the DMA. Re-allocating the VRF...");
		VR->re_vecreg(num_stream);
	}
	// Load the Vector Register with bytes from the DMA buffer.
	for(int i = 0; i < num_stream; i++)
		VR->VR_load(i, DMA_buffer[i], 0, stream_len, VecReg_cycle);

	return;
}

void DMA::VR_load(char *input_filename, int *gather_cycle, int *VecReg_cycle, int num_stream, Vec_Reg *Vector_Register)
{
	// Read streams into the DMA, then send them into the Vector Register
	this->read_streams(input_filename);
	this->send_streams(Vector_Register, VecReg_cycle);

	return;
}

void DMA::gather(char *input_filename)
{
	// Start the DMA clock
    clock_t begin = clock();

    // Initialize a cycle counter for the Vector Register.. probably should have just made this
    // a part of the class...erm...
	int *VecReg_cycle = (int *)malloc(sizeof(int));
	*VecReg_cycle = 0;

	// Start the loading process.
	this->VR_load(input_filename, &gather_cycle, VecReg_cycle, num_stream, Vector_Register);

	// Stop the DMA clock.
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

    // Report the DMA data
    MM->report_DMA(elapsed_secs, gather_cycle, *VecReg_cycle);

    // Cleanup
    free(VecReg_cycle);
    
	return;
}