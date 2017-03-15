# Unified-Automata-Processor
A repository containing my work from the Summer of 2015 on the Unified Automata Processor (UAP) in the Large Scale Systems Group at UChicago.

Use Instructions Follow:


 # The Unified Automata Processor: 		
 # A Method of Processing over Unstructured Data	

 ## By Kevin Fang and Willem Longendyke		

## The University of Chicago, Department of Computer Science		


Included in this code are methods to:
+ Import NFA/DFA/A-DFA from text format.
+ DFA/NFA/A-DFA traversal routines.

Build Information


The code can be compiled and built using the provided makefile. 
Targets:
+ all: builds the whole code (default)
+ clean: deletes all object files and executable

The code has been compiled using gcc 4.8.4 and under Ubuntu version 14.04

USAGE:
./UAP [options]
+ [-h]
+ [-p <automaton prefix>]
+ [-n <number of automata>]
+ [-t <automata type>]
+ [-i <input filename>]
+ [-m <lane mask>]
+ [-s <static mapping>]
+ [-c <configuration filename>]
+ [-l <step size>]
+ [-v <verbosity level>]

Options:
	+ -h,				 print this message
	+ -p <automaton prefix>,	 parse all automata with this prefix
	+ -n <number of automata>,	 parse this number of automaton
	+ -t <automata type>,		 the type of automata being parsed
	+ -i <input filename>,		 process over this input file
	+ -m <lane mask>,		 use this lane mask (use base 2)
	+ -s <static mapping [0-3]>,	 use this static mapping between the VRF and the DMA
	+ -c <configuration filename>,	 use this configuration file
	+ -l <step size>,		 use this step size
	+ -v <verbosity [0-3]>,		 use this verbosity level:
						 ..* 0: no messages.
						 ..* 1: messages from the DMA.
						 ..* 2: messages from the prefetcher.
						 ..* 3: messages from the sequence logic.

# Code Organization

	+ main.cpp
	..* Contains the main function, parsing command line arguments, and initializes the PMA, which starts the whole program.

	+ PMA.cpp, PMA.h
	..* Contains the PMA class, which calls the following large branches of the program (beginning the Load, Configure, Gather, and Traverse stages of the UAP). The PMA manages the UAP itself, as well as the DMA.

	+ DMA.cpp, DMA.h
	..* Contains the DMA class, which is responsible for loading input streams into the UAP's Vector Register File and for loading automata into the UAP's memory banks.

	+ UAP.cpp, UAP.h
	..* Contains the UAP class which holds all lanes, memory banks, Vector Register Files, prefetchers, sequence logic, and combination queues. Includes functions to configure the UAP and to traverse the input streams. 
	..* Also includes the lane class, which individually holds its own sequence logic, prefetcher, and combination queue. This class includes functions to configure each lane as well as traverst the input streams.

	+ lane_tools.cpp/lane_tools.h
	..* Contains the CBQ and sequence_logic classes, which are responsible for, respectively, containing the current state(s) the UAP is in and actually processing over the given state and symbol.

	+ mem_tools.cpp/mem_tools.h
	..* Contains the Vec_Reg, LM_Bank, and prefetcher classes. The Vec_Reg class is responsible for holding all the input streams and (eventually) copying those streams into each lane's prefetcher. The class contains functions to those ends.
	..* The next class, the LM_Bank, is responsible for keeping each automaton loaded into the UAP. The class includes functions to load this automata into the bank correctly, and to process a transition over that automata.
	..* Finally, the prefetcher class is responsible for feeding the sequence logic with input bytes from the VRF. It includes a function for this purpose.

	+ nfa.cpp, nfa.h, dfa.cpp, dfa.h, a_dfa.cpp, a_dfa.h
	..* These files contain the multiple derived classes representing the automata which the UAP uses to process data. They include functions for reading plaintext automata into those classes, and for operating over input expressions to return matchings.
	
	+ auto.cpp, auto.h
	..* Contains the base class which binds together the previous classes (nfa/dfa/a-dfa). Includes virtual functions which link to the respective derived classes' functions.

	+ metric.h, metric.cpp
	..* Contains the metric manager class, which is responsible for keeping track of various metrics during processing and for reporting those metrics at the end of processing. It includes functions for doing so.

	+ utils.cpp, utils.h
	..* Includes various functions for allocating arrays and printing messages, definitions of global variables, and type definitions as well as functions for operating over linked lists.

# How To Use the UAP with the RegEx Compiler

1. Run the RegEx Compiler over a set of patterns, having it output an automata of your choosing using the --nfa/--dfa/--a_dfa command line options
2. If using multiple automata, be sure to have them named with a base prefix, for example, for 5 automata:
	..*AUTO_0.txt
	..*AUTO_1.txt
	..*AUTO_2.txt
	..*AUTO_3.txt
	..*AUTO_4.txt
where AUTO is the base prefix (to be given to the UAP) and each _#.txt will be appended automatically by the UAP (to change this, see UAP.cpp, lines 76-85).
3. Load these automata into the UAP (only ONE type at a time) by specifying the base prefix, number of automata, automaton type, input filename, and the other listed options above.
