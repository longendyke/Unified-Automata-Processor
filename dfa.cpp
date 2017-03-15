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
 * File:   dfa.c
 * Author: Michela Becchi
 * Email:  mbecchi@cse.wustl.edu
 * Organization: Applied Research Laboratory
 * 
 */


#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctime>

#include "dfa.h"


using namespace std;

DFA::DFA(unsigned N)
{
	dead_state = NO_STATE;
	entry_allocated = N;
	size = 0;
	state_table = allocate_state_matrix(entry_allocated);
	accepted_rules = (linked_list **)allocate_array(entry_allocated,sizeof(linked_list *));
	labeled_tx=NULL;
	for (state_t s = 0; s < entry_allocated; s++){
		state_table[s]=NULL;
		accepted_rules[s]=NULL;
	}
	return;
}

DFA::~DFA()
{
	for(state_t i = 0; i < size; i++){
		free(state_table[i]);
		if (accepted_rules[i]) 
			delete accepted_rules[i];
	}
	free(state_table);	
	free(accepted_rules);
	if (labeled_tx!=NULL){
		for (state_t s = 0; s < size; s++)  
			delete labeled_tx[s];
		free(labeled_tx);
	}
	return;
}

state_t DFA::lookup(state_t s, symbol_t c)
{
	state_t next = NO_STATE;
	FOREACH_TXLIST(labeled_tx[s], it){
		if ((*it).first==c){
			next=(*it).second;
			break;
		}
	}
	if (next != NO_STATE)
		return next;
	else
		return 0; 	
}

//////////////////////////////////////////////////////////////
/////////////// BEGIN EDITED/SELF WRITTEN CODE ///////////////
//////////////////////////////////////////////////////////////


// Read the dfa from file.
void DFA::read_auto(FILE *file)
{
    // Start the reading clock
    clock_t begin = clock();

	//Initialize a counter, rewind file
    long posn;
    rewind(file);

    message("Begin resetting DFA\n");

    //Resetting the DFA
    for (unsigned int i = 0; i < size; i++) {
        free(state_table[i]); 
        delete accepted_rules[i];
        if (labeled_tx!=NULL) 
        	delete labeled_tx[i];
    }
    free(state_table);
    if (labeled_tx!=NULL) 
    	free(labeled_tx);
    free(accepted_rules);

    message("Done resetting DFA\n\t Begin reading from file\n");
    
    //***=========================***//
    //*** BEGIN READING FROM FILE ***//
    //***=========================***//

    message("Begin reading comment\n");

    // Read the comment
    char c = getc(file);
    if (c=='#'){
        while (c!='\n') c=getc(file);
    }else{
        rewind(file);
    }

    message("Done reading comment\n\t Begin reading # states\n");

    // Number of states
    int res=fscanf(file,"%d\n",&size);
    if (res==EOF || res<=0) {
        error("DFA:: read_auto: size not read");
    }

    printf("MESSAGE: %ld states\n\t Done reading # states\n\n", size);

    // Sanity Check
    if(size==0) 
    	return;

    message("Rebuilding DFA\n");

    // Set entry_allocated
    entry_allocated = size;
    
    // Allocate state table
    state_table = allocate_state_matrix(size);

    // Allocate labeled transitions
    labeled_tx = (tx_list **) allocate_array(size,sizeof(tx_list *));

    // Allocate accepted rules
    accepted_rules = (linked_list **) allocate_array(size,sizeof(linked_list *));

    message("Done rebuilding DFA\n\t Begin reading states\n");

    //***BEGIN READING STATES***//

    // Per State Information //
    //=======================//
    for (state_t s = 0; s < size; s++){
        symbol_t c;
        state_t ns;
        labeled_tx[s] = new tx_list();
        state_table[s] = allocate_state_array(ALPHABET_SIZE);
        accepted_rules[s] = new linked_list();

        res = fscanf(file, " ( ");

        posn = ftell(file);
        res = fscanf(file,"%d :%d ", &c, &ns);
        while(res != EOF && res > 0){
            //Store the transition into the labeled transitions
            labeled_tx[s]->push_back(tx_t(c, ns));
            posn = ftell(file);
            res = fscanf(file,"%d :%d ", &c, &ns);
        }

        // Seek the last place read
        fseek(file, posn, SEEK_SET);

        posn = ftell(file);
        res = getc(file); 

        // Error Checking
        if (res!=')'){
        	error("DFA:: read_auto: invalid format");
        }

        // Get accepted rule(s)
        state_t accept;
        posn = ftell(file);
        res = fscanf(file," acc: %d ", &accept);

        while(res != EOF && res > 0){
            accepted_rules[s]->insert(accept);
            posn=ftell(file);
            res=fscanf(file, "%d ", &accept);
        }

        if(res == 0){
        	res = fscanf(file,"acc:");
        	posn = ftell(file);
        	fseek(file, posn, SEEK_SET);
        	res = getc(file);

        	if (res!=';') 
	        	error("DFA:: read_auto: invalid format");

        	res = getc(file); 

        	if (res!='\n') 
        		error("DFA:: read_auto: invalid format");
        } else{
        	fseek(file, posn, SEEK_SET);
        	res = getc(file);

        	if (res!=';') 
        		error("DFA:: read_auto: invalid format");

        	res = getc(file); 

        	if (res!='\n') 
        		error("DFA:: read_auto: invalid format");
        }
    }

    message("Finished reading all states\n");

    //***END READING STATES***//

    message("Reconstructing State Table\n");

    // Reconstructing state table
    for (state_t s = 0; s < size; s++){
        for (int c = 0; c < ALPHABET_SIZE; c++){
            state_table[s][c] = lookup(s,c);
        }
    }

    // Stop the reading counter
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

    // Print a nice little summary for our little summary collection.
    printf("  DFA:: READ_AUTO SUMMARY\n");
    printf("/=============================\\\n");
    printf("  Finished Reading Automaton\n");
    printf("  %u States Created\n", size);
    printf("  %f Seconds Elapsed\n", elapsed_secs);
    printf("\\=============================/\n\n");

    return;
}

void DFA::dump(FILE *log){
	fprintf(log, "Dumping DFA: %u states\n", size);
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
    		if (state_table[i]!=NULL && state_table[i][j] != 0) 
	    		fprintf(log," - [%d/%c] %d\n", j, j, state_table[i][j]);
    	}
    }
}

void DFA::do_transition(symbol_t c, state_t s, int *match_num, int *cycle, int *buckets)
{
    // Get the next state from the transition table
    state_t *next_state = NULL;
    next_state = get_next_state(s, c);

    // Get accepted rules for the next state
    linked_list *acc_rules = accepts(*next_state);

    // If the accepted rules for this state is non-empty,
    // then we have at least one match
    if(!acc_rules->empty()){
        (*match_num)++;
    }

    // If the next state is accepting, increment the state's bucket counter
    unsigned int bucket = 0;
    while(!acc_rules->empty()){
        bucket = acc_rules->ret_data();
        if(buckets[bucket] == 0){
            (buckets[bucket])++;
        }
        acc_rules = acc_rules->ret_next();
    }

    // Increment the cycle counter
    (*cycle)++;

    return;
}

state_t *DFA::get_next_state(state_t state, symbol_t c) 
{	
    if(state_table[state][c] != (unsigned int) 0){
        return &(state_table[state][c]);
    }
    else{
        return &(state_table[0][c]);
    }
}

int DFA::ret_num_transitions() 
{
    int counter = 0;
    for(state_t s = 0; s < (unsigned) size; s++)
        counter += labeled_tx[s]->size();

    return counter;
}