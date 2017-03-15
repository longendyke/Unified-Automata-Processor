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

#include "lane_tools.h"

sequence_logic::sequence_logic(int num_states)
{
	// Simple initialization
	cycle = 0;
	match_num = 0;

	// Matches! Fire!
	// Actually for keeping track of which rule gets matched.
	// But also fire! Matches!
	matches = (int *) malloc(sizeof(int)*num_states);
	for(int i = 0; i < num_states; i++)
		matches[i] = 0;

	return;
}

sequence_logic::~sequence_logic()
{
	// Delete the matches. No fire.
	free(matches);

	return;
}

void sequence_logic::examine_bytes(symbol_t *prf_buf, int num_bytes, CBQ *cbq, LM_Bank memory)
{
	// Switch on the type of automata	
	unsigned int type = memory.type;

	// Initialize a counter to keep track of where we are on the prefetcher buffer
	int counter;
	switch(type){
		// NFA type
		case 1:
		{
			// Loop over all the bytes we've received from the prefetcher
			for(counter = 0; counter < num_bytes; counter++){
				
				// Error checking
				int match_num_temp = match_num;

				// Store our current states in a more friendly variable
				// Since we're working in an NFA this will almost certainly be a list of states
				state_t *curr_states = cbq->q_in;

				// Loop over all of the queued states in the CBQ for the given symbol
				for(int j = 0; curr_states[j] != NO_STATE; j++){
					// Advance the cycle count
					cycle++;
					memory.examine(curr_states[j], prf_buf[counter], &match_num, &cycle, matches);
				}

				// Error and double-counting check
				if(match_num == match_num_temp+2)
					match_num--;
				
				// Retrieve all next states	
				state_t *all_next = memory.automaton->retrieve_all_next(curr_states, prf_buf[counter]);

				// Count how many states we are sending to the CBQ
				int tot_states = 0;
				for(int i = 0; all_next[i] != NO_STATE; i++){
					tot_states++;
				}

				// Send our states to the CBQ
				cbq->put_states(all_next, tot_states);

				//Cleanup
				free(all_next);

				// Switch the queues
				cbq->switch_qin_qout();
			}
			break;
		}
		// DFA type
		case 2: 
		{
			for(counter = 0; counter < num_bytes; counter++){
				// Store our current states in a more friendly variable
				// Since we're working in a DFA this should be a single state
				state_t *curr_state = cbq->q_in;
				
				// Advance the cycle count.
				cycle++;

				// Examine the bytes in the prefetcher
				memory.examine(*curr_state, prf_buf[counter], &match_num, &cycle, matches);

				// Retrieve all next states	
				curr_state = memory.automaton->get_next_state(*curr_state, prf_buf[counter]);			

				// Send our states to the CBQ
				cbq->put_states(curr_state, 1);

				// Switch the queues
				cbq->switch_qin_qout();
			}
			break;
		}
		// A_DFA type
		case 3: 
		{
			for(counter = 0; counter < num_bytes; counter++){
				
				// Store our current states in a more friendly variable
				// Since we're working in a DFA this should be a single state
				state_t *curr_state = cbq->q_in;

				// Advance the cycle count.
				cycle++;

				// Examine the bytes in the prefetcher
				memory.examine(*curr_state, prf_buf[counter], &match_num, &cycle, matches);
				
				// Retrieve all next states	
				curr_state = memory.automaton->get_next_state(*curr_state, prf_buf[counter]);
				
				// Send our states to the CBQ
				cbq->put_states(curr_state, 1);

				// Switch the queues
				cbq->switch_qin_qout();
			}
			break;
		}
		default: error("PMA::PMA_run: Unknown automata type");
				break;
	}
}


CBQ::CBQ(int new_q_size, unsigned int start_state)
{
	// Simple initializations
	q_size = new_q_size;

	// Start by setting the initial state to 0 (just for kicks).
	q_in = (state_t *) malloc(sizeof(state_t) * q_size);
	for(int i = 0; i < q_size; i++){
		if(i == 0){
			q_in[i] = start_state;
			continue;
		}
		q_in[i] = NO_STATE;
	}

	// Leave the out-queue empty (to be filled in by CBQ::send_states)
	q_out = NULL;

	return;
}

CBQ::~CBQ()
{
	// q_out should have already been de-allocated by this point.
	free(q_in);
	return;
}

void CBQ::put_states(state_t *states, int num_states)
{
	// Allocate a new out-queue and initialize it.
	state_t *n_q = (state_t *) malloc(q_size * sizeof(state_t));
	for(int j = 0; j < q_size; j++)
		n_q[j] = NO_STATE;
	
	// Transfer over the states.
	for(int i = 0; i < num_states; i++){
		n_q[i] = states[i];
	}

	// Actually set q_out to those new states.
	q_out = n_q;

	return;
}

void CBQ::switch_qin_qout()
{
	// Don't need this one anymore.
	free(q_in);
	
	state_t *n_q = (state_t *) malloc(q_size * sizeof(state_t));
	for(int j = 0; j < q_size; j++)
		n_q[j] = NO_STATE;
	q_in = n_q;

	// Copy the out-queue into the in-queue
	q_in = (state_t *) memcpy(q_in, q_out, q_size * sizeof(state_t));

	// De-allocate the out-queue (to be re-allocated by send_states)
	free(q_out);

	return;
}