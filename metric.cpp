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

#include "metric.h"
#include "math.h"

using namespace std;

metric_manager::metric_manager(unsigned int verb, unsigned int stp, int rnd, int lane)
{
	// Simple initializations
	verbosity = verb;
	step = stp;
	round = rnd;
	lane_num = lane;
	overall_cycle = 0;
	overall_time = 0;
	number_characters = 0;

	return;
}

metric_manager::~metric_manager()
{
	// lol
	return;
}

void metric_manager::report_DMA(double elapsed_secs, int gather_cycle, int VR_cycle)
{
	// Verbosity check
	if(verbosity < 1)
		return;

	printf("\n\n////////////////////DMA REPORT\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
	printf("  ++ DMA cycles run ---> \t\t%d.\n", gather_cycle);
	printf("  ++ Vector Register cycles run ---> \t%d.\n", VR_cycle);
	printf("  ----------------------------------------------\n");
	printf("  == Step size: %u.\n", step);
	printf("  == Total milliseconds elapsed: %.2f.\n", elapsed_secs*1000);
	float result = (elapsed_secs / ((float)gather_cycle * (float)step))*1000000000;
	printf("  == Nanoseconds per symbol for each cycle: %.2f\n", result);
	float clockspeed = (float)(gather_cycle/elapsed_secs)* 1/1000;
	printf("  == DMA clock: %.1fkHz.\n", clockspeed);
	float VRclockspeed = (float)(VR_cycle/elapsed_secs)* 1/1000;
	printf("  == VRF clock: %.1fkHz.\n", VRclockspeed);
	printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/////////////////////////\n\n");

	return;
}

void metric_manager::report_prefetch(double elapsed_secs, int prf_cycle)
{
	// Verbosity check
	if(verbosity < 2)
		return;

	printf("\n////////////////////PREFETCHER REPORT\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
	printf("  == Prefetcher cycles run: %d\n", prf_cycle);
	printf("  == Step size: %u.\n", step);
	printf("  == Total milliseconds elapsed: %.2f.\n", elapsed_secs*1000);
	float clockspeed = (float)(prf_cycle/elapsed_secs)* 1/1000;
	printf("  == Prefetcher clock: %.1fkHz.\n", clockspeed);
	printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/////////////////////////////\n\n");

	return;
}

void metric_manager::report_sequence(double elapsed_secs, int cycle, int match_num, int *matches, int num_states)
{
	// Verbosity check
	if(verbosity < 3)
		return;

	if(cycle > overall_cycle)
		overall_cycle = cycle;

	// if(elapsed_secs > overall_time)
	overall_time += elapsed_secs;

	printf("\n////////////////////LANE REPORT\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
	printf("  == Sequence Logic cycles run: %d\n", cycle);
	printf("  == Total milliseconds elapsed: %.2f.\n", elapsed_secs*1000);
	float clockspeed = (float)(cycle/elapsed_secs)* 1/1000;
	printf("  == Clock: %.1fkHz.\n", clockspeed);
	printf("  ----------------------------------------------\n");
	printf("  == Number of matches: %d.\n", match_num);
	int buck_count = 0;
	for(int B = 0; B < num_states; B++)
		buck_count += matches[B];
	printf("  == Number of rules matched: %d.\n", buck_count);
	printf("  == Rules matched:\n\t\t\t[");

	// Figuring out which rules were found.
	int placeholder = -1;
	bool succession = false;
	for(int i = 0; i < num_states; i++){
		// If this test passes then there was at least one match
		if(matches[i] != 0){
			// If this is true, then we've found non-consecutive rules which were matched
			if(placeholder != (i - 1)){
				// If this passes then this was the last in a group of consecutive matched states
				if(succession)
					printf(" %d", placeholder);
				// Print the rule we just matched and reset the consecutive boolean
				printf(" %d", i);
				succession = false;
			}
			else{
				// If we're here then we've found consecutive matches
				if(!succession)
					printf(" -");
				succession = true;
			}
			// Keep track of our last match.
			placeholder = i;
		}
	}
	// Make sure to print the last rule we found if we ended in consecutive matches!
	if(succession)
		printf(" %d" , placeholder);
	
	printf(" ]\n");
	printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\///////////////////////////\n\n");

	return;
}

void metric_manager::report_final()
{
	printf("\n////////////////////FINAL REPORT\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
	printf("  == Number of characters processed: %i\n", number_characters);
	printf("  == Overall Time: %.1f milliseconds\n", overall_time*1000);
	double clock_speed = 0;
	clock_speed = (double) (number_characters/overall_time)*1/1000;
	printf(" Final Clock Speed: %.1fkHz\n", clock_speed);
	printf("//////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
	return;
}