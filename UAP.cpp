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

#include "UAP.h"

UAP::UAP(int num_lane, int mem_num, int step)
{

	// Initialization
	lane_num = num_lane;
	num_mem = mem_num;
	UAP_step = step;

	// Initialize the array of UAP lanes
	lanes = (lane *)malloc(num_lane * sizeof(lane));
	for(int i = 0; i < num_lane; i++){
		lanes[i] = *(new lane(0, false));
	}

	// WARNING::These memory banks need to be specified elsewhere, because we need 
	// the type of automata that each bank will hold.
	memory = (LM_Bank *)malloc(num_mem * sizeof(LM_Bank));

	// Initialize the Vector Register
	VRF = new Vec_Reg(64, step);

	// Leave the Metric Manager pointer as NULL, for now.
	MM = NULL;

	return;
}

UAP::~UAP()
{
	// Delete the lanes
	for(int i = 0; i < lane_num; i++){
		delete &(lanes[i]);
	}

	// Delete the memory banks
	for(int j = 0; j < num_mem; j++){
		delete &(memory[j]);	
	}

	// Delete the VRF
	delete VRF;

	return;
}

void UAP::load_automata(char *base_file_prefix, int file_num, int auto_type)
{

	// Initialize the LM_Bank
	for(int ind = 0; ind < num_mem; ind++)
		memory[ind] = *(new LM_Bank(auto_type));

	// Sanity check
	if(file_num > num_mem)
		error("UAP::load_automata: Error, given too many files for the UAP's memory");

	for(int i = 0; i < file_num; i++){
		// String to be appended to the base file prefix
		char *temp = (char *) malloc(sizeof(char)* 25);
		sprintf(temp, "_%d", i);

		// To be passed to automata->read(FILE *file_name)
		char *filename = (char *) malloc(100 * sizeof(char));

		// Appending to the filename
		strcpy(filename, base_file_prefix);
		strcat(filename, temp);
		strcat(filename, ".txt");

		// Load the automaton into our memory banks
		memory[i].load_automaton(filename, auto_type);

		// Cleanup
		free(filename);
		free(temp);
	}

	return;
}

void UAP::read_config_file(char *filename)
{
	// Open the config file and check
	FILE *file = fopen(filename, "r");
	if(file == NULL)
		error("Could not open config file.");

	// Strings to be read into when reading from the config file
	char *line = (char *)malloc(sizeof(char) * 15);
	char *command = (char *)malloc(sizeof(char) * 8);
	int value = 0;

	// More dummy initializations
	int init_state = 0;
	unsigned long long int lane_mask = 0xFFFFFFFFFFFFFFFF;
	int vec_sharing = 0;

	// Actually begin to read in cofiguration settings
	fgets(line, 15, file);
	while(!feof(file)){
		sscanf(line, "%s %i", command, &value);
		int switcher = 0;
		if(strcmp(command, "LANENUM") == 0)
			switcher = 1;
		if(strcmp(command, "BANKNUM") == 0)
			switcher = 2;
		if(strcmp(command, "STATMAP") == 0)
			switcher = 3;
		if(strcmp(command, "LANEMSK") == 0)
			switcher = 4;
		if(strcmp(command, "INSTATE") == 0)
			switcher = 5;

		switch(switcher){
			case 0:
				error("UAP::switch_command: INVALID COMMAND");
				break;
			case 1:
				lane_num = (int) value;
				break;
			case 2:
				num_mem = (int) value;
				break;
			case 3:
				vec_sharing = (int) value;
				break;
			case 4:
				lane_mask = (unsigned long long int) value;
				break;
			case 5:
				init_state =  (int) value;
				break;
		}
	fgets(line, 15, file);
	}

	// Sanity checks
	if(lane_num != num_mem)
		error("UAP::read_config_file: The number of lanes is not equal to the number of memory banks.");
	if(vec_sharing < 0 || vec_sharing > 3)
		error("UAP::read_config_file: The static mapping must be between 0 and 3.");
	if(init_state < 0)
		error("UAP::read_config_file: Invalid initial state.");

	// Close the file.
	fclose(file);
	
	// Configure the UAP with these settings.
	UAP_config(init_state, lane_mask, vec_sharing);

	// Cleanup
	free(command);
	free(line);

	return;
}


void UAP::UAP_config(int init_state, unsigned long long int lane_mask, int vec_sharing)
{
	// Set all mask booleans for each lane
	unsigned long long int masker = 1;

	int mem = 0;

	for(int i = 0; i < lane_num; i++, mem++){

		// if(mem >= num_mem){
		// 	message("~~~WARNING~~~UAP::UAP_config: more lanes than automata.");
		// 	message("Wrapping around the memory");
		// 	mem = 0;
		// }

		// Get the automaton type for each lane
		unsigned int lane_auto_type = memory[mem].get_type();

		// Get the automata for each lane, as well as the total number of states
		automata *automaton = memory[mem].get_automata();
		int num_states = automaton->ret_num_states();

		// Quick sanity check
		if(init_state > num_states)
			error("UAP::UAP_config: Invalid initial state.");

		// Initialize all parts of the lane.
		sequence_logic *new_seq = new sequence_logic(num_states);
		CBQ *new_cbq = new CBQ(num_states, (unsigned int) init_state);
		prefetcher *new_prf = new prefetcher(UAP_step);

		// Mask test
		bool masked;
		int to_mask = masker & lane_mask;
		if(!!to_mask)
			masked = true;
		else
			masked = false;

		// Validity checking of our masking unsigned int
		unsigned long long int checker = masker;
		masker*=2;

		// If this test passes then we have wrapped around our 
		// unsigned long long int (ie we have more than 64 lanes)
		if(masker < checker)
			masker = 1;

		// Actually configure the current lane
		lanes[i].lane_config(new_prf, new_cbq, new_seq, vec_sharing, masked, init_state, MM);
	}
	return;
}

void UAP::traverse(int start, int step)
{
	// Start each lane traversal
	for(int i = 0; i < lane_num; i++){
		printf("LANE #%d\n", i);
		lanes[i].traverse(start, step, memory[i], VRF);
	}
	return;
}

lane::lane(unsigned int i_state, bool i_mask)
{
	// Simple initializations (many of which to be completed
	// during UAP_config and lane_config instead of here).
	seq = NULL;
	cbq = NULL;
	prf = NULL;

	init_state = i_state;
	mask = i_mask;

	vec_sharing = 0;
	MM = NULL;

	return;
}

lane::~lane()
{
	// Delete ALL the things!
	delete seq;
	delete cbq;
	delete prf;

	return;
}

void lane::lane_config(prefetcher *the_prf, CBQ *the_cbq, sequence_logic *the_seq, int nvec_sharing, bool nmask, int ninit_state, metric_manager *metric)
{
	// Fill in all the blanks from initialization.
	seq = the_seq;
	cbq = the_cbq;
	prf = the_prf;

	vec_sharing = nvec_sharing;
	mask = nmask;
	init_state = ninit_state;

	MM = metric;

	return;
}

void lane::traverse(int start, int step, LM_Bank memory, Vec_Reg *VRF)
{
	// Counters for the amount of time passed
	double prf_total_secs = 0;
	double seq_total_secs = 0;

	// Check to make sure that this lane is masked to be run over
	// then actually traverse the input streams.
	if(mask){
		for(int i = 0; i < VRF->ret_numVR(); i++){
			// Determine which vector register we're getting our input from.
			// If it's the one we're currently on, the next test should pass.
			int dst_vrf = i % vec_sharing;
			if(!(dst_vrf)){
				
				// Start the prefetcher clock
				clock_t prf_begin = clock();

				// Actually start the prefetcher
				prf->prefetch(VRF, i, prf->size, 0);

				// End the prefetcher clock and calculate the total time spent on the prefetcher.
				clock_t prf_end = clock();
				double prf_elapsed_secs = double(prf_end - prf_begin) / CLOCKS_PER_SEC;
				prf_total_secs += prf_elapsed_secs;

				// Start the sequence logic clock
				clock_t seq_begin = clock();

				// Examine the bytes in the prefetcher
				seq->examine_bytes(prf->buffer, prf->size, cbq, memory);
				
				// End the prefetcher clock and calculate the total time spent on the sequence logic
				clock_t seq_end = clock();
				double seq_elapsed_secs = double(seq_end - seq_begin) / CLOCKS_PER_SEC;
				seq_total_secs += seq_elapsed_secs;
			}
		}
		// Report all our data on the prefetcher and the sequence logic (if the verbosity is set).
    	MM->report_prefetch(prf_total_secs, prf->cycle);
    	MM->report_sequence(seq_total_secs, seq->cycle, seq->match_num, seq->matches, (int)memory.automaton->ret_num_states());
	} else {
		message("Lane not masked!");
	}

	MM->report_final();
	return;
}