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
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "dfa.h"
#include "a_dfa.h"
#include "nfa.h"
#include "PMA.h"
#include "DMA.h"
#include "auto.h"
#include "mem_tools.h"
#include "UAP.h"
#include "lane_tools.h"
#include "metric.h"

using namespace std;

int main(int argc, char** argv)
{
    int opt;

    // File name of the automaton
    char *prefix = NULL;

    // Number of automata
    int num_auto = 0;

    // Integer for the automaton type:
    // NFA = 1; DFA = 2; A-DFA = 3
    int auto_type = 0;

    // The lane mask
    long long int mask = 0;

    // The UAP's static mapping
    int static_map = 0;

    // The UAP's config filename
    char *config = NULL;

    // The input file to be loaded
    char *input = NULL;

    // The step size (will map to prefetcher size, VRF size, and DMA size)
    int step = 256;

    // The verbosity for the Metric Manager
    unsigned int verbosity = 0;

    prefix = (char *) malloc(sizeof(char) * 256);
    if(prefix == NULL){
        std::cout << "ERROR: Could not allocate memory";
        exit(-1);
    }
    
    while((opt = getopt(argc, argv, "p:t:n:i:m:s:c:l:hv:")) != -1){
        switch (opt)
        {
            case 'p':
                prefix = strdup(optarg);
                break;
            case 'n':
            {
                num_auto = strtol(optarg, NULL, 10);
                break;
            }
            case 't':
            {
                auto_type = strtol(optarg, NULL, 10);
                break;
            }
            case 'i':
                input = strdup(optarg);
                break;
            case 'm':
                mask = strtoll(optarg, NULL, 2);
                break;
            case 's':
            {
                static_map = strtol(optarg, NULL, 10);
                break;
            }
            case 'c':
            	config = strdup(optarg);
            	break;
            case 'l':
            {
            	step = strtol(optarg, NULL, 10);
            	break;
            }
            case 'v':
            {
            	verbosity = strtol(optarg, NULL, 10);
            	break;
            }
            case 'h':
            {
            	printf("USAGE:\n");
            	printf("UAP [options]\n");
            	printf("\t[-h]\n");
            	printf("\t[-p <automaton prefix>]\n");
            	printf("\t[-n <number of automata>]\n");
            	printf("\t[-t <automata type>]\n");
            	printf("\t[-i <input filename>]\n");
            	printf("\t[-m <lane mask>]\n");
            	printf("\t[-s <static mapping>]\n");
            	printf("\t[-c <configuration filename>]\n");
            	printf("\t[-l <step size>]\n");
            	printf("\t[-v <verbosity level>]\n\n");
            	printf("Options:\n");
            	printf("\t-h,\t\t\t\t print this message\n");
            	printf("\t-p <automaton prefix>,\t\t parse all automata with this prefix\n");
            	printf("\t-n <number of automata>,\t parse this number of automaton\n");
            	printf("\t-t <automata type>,\t\t the type of automata being parsed\n");
            	printf("\t-i <input filename>,\t\t process over this input file\n");
            	printf("\t-m <lane mask>,\t\t\t use this lane mask (use base 2)\n");
            	printf("\t-s <static mapping [0-3]>,\t use this static mapping between the VRF and the DMA\n");
            	printf("\t-c <configuration filename>,\t use this configuration file\n");
            	printf("\t-l <step size>,\t\t\t use this step size\n");
            	printf("\t-v <verbosity [0-3]>,\t\t use this verbosity level:\n");
            	printf("\t\t\t\t\t\t 0: no messages.\n");
            	printf("\t\t\t\t\t\t 1: messages from the DMA.\n");
            	printf("\t\t\t\t\t\t 2: messages from the prefetcher.\n");
            	printf("\t\t\t\t\t\t 3: messages from the sequence logic.\n");
            	return 0;
            }
            default:
                printf("ERROR: Unknown option -%c\n", opt);
                exit(-1);
        }
    }

    PMA *pma = NULL;
    // ...don't ask... //
    int sixty_four = 64;
    unsigned one = 1;

    // Create the new PMA
    pma = new PMA(one, sixty_four, sixty_four, sixty_four, (unsigned int) step, verbosity);

    // Check some of the arguments (other checks are above)
    if(prefix == NULL)
    	error("No prefix provided!");
    if(input == NULL)
    	error("No input provided!");
    if(auto_type < 1 || auto_type > 3)
        error("Invalid automaton type.");
    if(num_auto <= 0)
		error("Invalid number of automata.");
	if(static_map > 3 || static_map < 0)
    	error("Static Mapping must be between 0 and 3");
    if(step <= 0)
		error("Invalid step size.");
	if(verbosity > 3 || verbosity < 0)
		error("Verbosity must be between 0 and 3");
    
    // Start the program!
    pma->PMA_run(config, prefix, num_auto, auto_type, input, step);

    // Cleanup
    free(prefix);
    free(input);
    free(config);

    return 0;
}