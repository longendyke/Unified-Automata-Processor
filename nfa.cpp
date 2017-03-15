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

/*
 * Copyright (c) 2007 Michela Becchi and Washington University in St. Louis.
 * All rights reserved
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. The name of the author or Washington University may not be used
 *       to endorse or promote products derived from this source code
 *       without specific prior written permission.
 *    4. Conditions of any other entities that contributed to this are also
 *       met. If a copyright notice is present from another entity, it must
 *       be maintained in redistributions of the source code.
 *
 * THIS INTELLECTUAL PROPERTY (WHICH MAY INCLUDE BUT IS NOT LIMITED TO SOFTWARE,
 * FIRMWARE, VHDL, etc) IS PROVIDED BY  THE AUTHOR AND WASHINGTON UNIVERSITY
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR WASHINGTON UNIVERSITY
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS INTELLECTUAL PROPERTY, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * */

/*
 * File:   nfa.c
 * Author: Michela Becchi
 * Email:  mbecchi@cse.wustl.edu
 * Organization: Applied Research Laboratory
 */

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctime>

#include "nfa.h"


using namespace std;

NFA::NFA(unsigned N)
{
	size = 0;

	entry_allocated = N;

	state_table = allocate_nfa_state_matrix(entry_allocated);

	accepted_rules = (linked_list **)allocate_array(entry_allocated, sizeof(linked_list *));

	labeled_transitions = NULL;

	for (state_t s = 0; s < entry_allocated; s++){
		state_table[s]=NULL;
		accepted_rules[s]=NULL;
	}

	return;
}

NFA::~NFA()
{
	for(state_t i = 0; i < (unsigned) size; i++){
		for(state_t ii = 0; ii < ALPHABET_SIZE; ii++){
			free(state_table[i][ii]);
		}
		free(state_table[i]);
		if (accepted_rules[i]) 
			delete accepted_rules[i];
	}
	free(state_table);	
	free(accepted_rules);
	if (labeled_transitions!=NULL){
		for (state_t s = 0; s < (unsigned) size; s++)  
			delete labeled_transitions[s];
		free(labeled_transitions);
	}
	return;
}

state_t* NFA::lookup(state_t s, symbol_t c, unsigned int size)
{
	state_t *next = NULL;

	// Failure value (filled with NO_STATE)
	// in case lookup fails
	state_t *failure = (state_t*) malloc(1 * sizeof(state_t));
	failure[0] = NO_STATE;

	// Simple counter in case of multiple transitions for
	// one character
	int count = 0;

	// Loop over all transitions for a given state, s, and
	// check if there exists a transition for a given symbol, c.
	FOREACH_TXLIST(labeled_transitions[s], it){
		if ((*it).first==c){
			// Add this transition to the list of next states to be given to the state table
			next = (state_t *) realloc(next, (count+2) * sizeof(state_t));
			next[count] = (*it).second;
			
			// Placeholder (like a '\0' for strings)
			next[count+1] = NO_STATE;
			
			// Advance counter in case of the existence of more transitions with input c
			count++;
		}
	}
	if (next != NULL){
		free(failure);
		return next;
	}
	else
		return failure;
}

void NFA::read_auto(FILE* file)
{
	// Start reading clock
	clock_t begin = clock();

	// Initialization of a counter and resetting the file
	long posn;
	rewind(file);

	message("Begin resetting NFA\n");

	// Reset the NFA
    for (unsigned int i = 0; i < (unsigned) size; i++) {
    	for(state_t ii = 0; ii < (unsigned) size; ii++){
			free(state_table[i][ii]);
		}
        free(state_table[i]); 
        delete accepted_rules[i];
        if (labeled_transitions!=NULL) 
        	delete labeled_transitions[i];
    }
    free(state_table);
    if (labeled_transitions!=NULL) 
    	free(labeled_transitions);
    free(accepted_rules);

    message("Done resetting NFA\n\t Begin reading from file\n");

	//***=========================***//
	//*** BEGIN READING FROM FILE ***//
	//***=========================***//

    message("Begin reading comment\n");

	// Read the comment
	char c = getc(file);
	if (c == '#'){
		while (c != '\n') 
			c = getc(file);
	}else{
		rewind(file);
	}

	message("Done reading comment\n\t Begin reading # states\n");

	// Number of states
	int res = fscanf(file, "%d\n", &size);
	if (res == EOF || res <= 0) {
		error("NFA:: read_auto: size not read");
	}

	printf("MESSAGE: %ld states\n\t Done reading # states\n\n", size);

	// Sanity Check
	if(size == 0) 
		return;

	message("Rebuilding NFA\n");

	// Rebuild the NFA
	entry_allocated = size;

	// State Table:
	state_table = allocate_nfa_state_matrix(size);
	for(state_t ind1 = 0; ind1 < size; ind1++){
		state_table[ind1] = (state_t**) allocate_array(ALPHABET_SIZE, sizeof(state_t*));
	}

	// Labeled Transitions:
	labeled_transitions = (tx_list **) allocate_array(size, sizeof(tx_list*));
	for(state_t index = 0; index < size; index++){
		labeled_transitions[index] = new tx_list();
	}

	// Accepting States:
	accepted_rules = (linked_list **) allocate_array(size, sizeof(linked_list **));
	for(state_t index = 0; index < size; index++){
		accepted_rules[index] = new linked_list();
	}

	message("Done rebuilding NFA\n\t Begin reading states\n");

	//***BEGIN READING STATES***//

	// Per State Information //
	//=======================//
	for(state_t s = 0; s < (unsigned) size; s++){

		// Initializing variables to hold information read
		unsigned int c = 0;
		unsigned int ns, current = 0;

		// Check if the state is accepting or not
		res = fscanf(file, "{ %u ", &current);
		int temp = 0;
		state_t accept;
		res = fscanf(file, "acc:%u ", &accept);

		// If res is a failure (<= 0), state is not acepting.
		// Else, add accepting state to known accepting states.
		if(res <= 0){
			res = fscanf(file, "{ %u \n", &current);
		} else {
			while(res != EOF && res > 0){
            	accepted_rules[current]->insert(accept);
            	posn=ftell(file);
            	res=fscanf(file, "%d ", &accept);
        	}
		}

		// Start reading transitions from each set of transitions
		posn = ftell(file);
        res = fscanf(file,"  ( :%u %u ", &ns, &c);

        STATE_SET:
        while(res != EOF && res > 0){
        	// Add the first transition to the list of labeled transitions,
        	// then continue reading to see if there are more characters leading
        	// to the current next_state
            labeled_transitions[current]->push_back(tx_t(c, ns));
        	posn = ftell(file);
            res = fscanf(file,"%d ", &c);
        }
        
        // Seek the last place read
        fseek(file, posn, SEEK_SET);

        // Checking if we need to read another set of state transitions
        posn = ftell(file);
        res = getc(file);
        // Res should be a ')' if we're really done with the set of character
        // transitions for this particular next_state

        // If this test passes, then the curent state (current) did not have
        // any transitions at all, and hence we don't need to move res
        // any farther forward.
        if(!(labeled_transitions[current]->empty())){
        	res = getc(file);
        	res = getc(file);
        	// res should be a '}' if we've reached the end of a state.
        	// If not, there's another set of transitions to be read (see if statement below)
        }

        if(res != '}'){
        	// If running here, then there is another set of state transitions 
        	// to read for our state.
        	posn = ftell(file);
        	fseek(file, posn, SEEK_SET);
        	
        	res = fscanf(file, " ( :%u %u ", &ns, &c);
        	if(res <= 0)
        		error("NFA:: read_auto: invalid format");
        	goto STATE_SET;
        } else {
        	posn = ftell(file);
        	fseek(file, posn, SEEK_SET);

        	if(res != EOF && res != '}')
        		error("NFA:: read_auto: invalid format");
        	res = getc(file);
        	// Should be a '\n' if we've finished one state and still have others,
        	// or, if completely finished, res will be EOF.
        }
	}

	message("Finished reading all states\n");

	//***END READING STATES***//

	message("Reconstructing State Table\n");

	// Reconstruct the State Table
	for (state_t s = 0; s < (unsigned) size; s++){
        for (int c = 0; c < ALPHABET_SIZE; c++)
            state_table[s][c] = lookup(s, c, size);   
    }

    // Stop the reading timer
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	// Print a nice little summary for all little summary needs.
	printf("  NFA:: READ_AUTO SUMMARY\n");
	printf("/=============================\\\n");
	printf("  Finished Reading Automaton\n");
	printf("  %u States Created\n", size);
	printf("  %f Seconds Elapsed\n", elapsed_secs);
	printf("\\=============================/\n\n");

	return;
}

void NFA::dump(FILE *log){
	fprintf(log, "Dumping NFA: %u states\n", size);
    for(state_t i = 0; i < size; i++){
    	fprintf(log, "> state # %d", i);
    	if (!accepted_rules[i]->empty()) {
    		fprintf(log," accepts ");
    		linked_list *s = accepted_rules[i];
    		while (s != NULL){
    			if(s->ret_data() != INVALID) 
    				fprintf(log, "[%u]", s->ret_data());
    			s = s->ret_next();
    		}
    	}
    	fprintf(log,"\n");
    	for(int j = 0; j < ALPHABET_SIZE; j++){
    		for(int I = 0; I < size; I++){
    			if (state_table[i]!=NULL && state_table[i][j][I] >= 0){
    				if(state_table[i][j][I]==NO_STATE)
    					break;
	    			fprintf(log," - [%d/%c:%d] %u\n", i, j, I, state_table[i][j][I]);
	    		}
    		}
    	}
    }
}

// Processes a transition
void NFA::do_transition(symbol_t c, state_t s, int *match_num, int *cycle, int *buckets)
{
	// Get the next state(s) from the transition table
	state_t *next_states = get_next_state(s, c);
	
	// Error checking, I swear.
	int tester = 0;

	//Loop through the next states
	for(int i = 0; next_states[i]!=INVALID; i++){
		// Get accepted rules for the next state
		linked_list *acc_rules = accepts(next_states[i]);

		// If the next state accepts the character just inputed,
    	// then we have a match (increment the match counter)
    	if(!acc_rules->empty()){
    		tester++;
    		if(tester == 1)
            	(*match_num)++;
        }

        // If the next state is accepting, increment the state's bucket counter
        int bucket;
        while(!acc_rules->empty()){
        	bucket = acc_rules->ret_data();
        	if(buckets[bucket] == 0)
        		(buckets[bucket])++;
        	acc_rules = acc_rules->ret_next();
        }

	}

    // Increment the cycle counter
	(*cycle)++;

	return;
}

state_t *NFA::retrieve_all_next(state_t *current_states, symbol_t c)
{	
	state_t *next_states = (state_t *) allocate_array(size + 1, sizeof(state_t));
	for(int t = 0; t < size; t++){
		next_states[t] = NO_STATE;		
	}
	
	// An index for indexing.
	int next_state_index = 0;

	// Loop over the current states
	for(int i = 0; current_states[i] != NO_STATE; i++){

		// Get the next states for the current state
		state_t *temp = get_next_state(current_states[i], c);

		// Copy the next states into the next_states array, checking first
		// to be certain that we aren't duplicating any of the next states
		for(int j = 0; temp[j] != NO_STATE; j++){			
			// Loop over the already found next states to make sure we aren't
			// duplicating any next states
			for(int k = 0; next_states[k] != NO_STATE; k++){				
				if(next_states[k] == temp[j])
					goto DUPLICATE;
			}

			next_states[next_state_index] = temp[j];
			next_state_index++;
			DUPLICATE:
			continue;
		}
	}
	
	next_states[next_state_index+1] = NO_STATE;

	return next_states;
}

state_t *NFA::get_next_state(state_t state, symbol_t c) 
{
    return state_table[state][c];
}

int NFA::ret_num_transitions() 
{
	int counter = 0;
	for(state_t s = 0; s < (unsigned) size; s++)
		counter += labeled_transitions[s]->size();

	return counter;
}