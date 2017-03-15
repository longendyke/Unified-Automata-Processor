OBJS = main.o utils.o dfa.o a_dfa.o nfa.o PMA.o metric.o DMA.o mem_tools.o auto.o UAP.o lane_tools.o
CC = g++
DEBUG = -g
CFLAGS = -std=c++11 -Wall -c $(DEBUG)
LFLAGS = -std=c++11 -Wall $(DEBUG)
RM = rm -f *~

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o UAP -lm

utils.o: utils.h utils.cpp
	$(CC) $(CFLAGS) utils.cpp

dfa.o: dfa.h dfa.cpp utils.h utils.cpp auto.h
	$(CC) $(CFLAGS) dfa.cpp

a_dfa.o: a_dfa.h a_dfa.cpp utils.h utils.cpp auto.h
	$(CC) $(CFLAGS) a_dfa.cpp

nfa.o: nfa.h nfa.cpp utils.h utils.cpp auto.h
	$(CC) $(CFLAGS) nfa.cpp

PMA.o: PMA.h PMA.cpp nfa.h dfa.h a_dfa.h auto.h
	$(CC) $(CFLAGS) PMA.cpp	

metric.o: metric.h metric.cpp UAP.h utils.h nfa.h dfa.h a_dfa.h auto.h mem_tools.h lane_tools.h DMA.h PMA.h
	$(CC) $(CFLAGS) metric.cpp

DMA.o: DMA.h DMA.cpp nfa.h dfa.h a_dfa.h auto.h utils.h metric.h
	$(CC) $(CFLAGS) DMA.cpp	

mem_tools.o: mem_tools.h mem_tools.cpp nfa.h dfa.h a_dfa.h auto.h
	$(CC) $(CFLAGS) mem_tools.cpp

auto.o: auto.h auto.cpp utils.h
	$(CC) $(CFLAGS) auto.cpp

UAP.o: UAP.h UAP.cpp utils.h nfa.h dfa.h a_dfa.h auto.h mem_tools.h DMA.h PMA.h
	$(CC) $(CFLAGS) UAP.cpp

lane_tools.o: lane_tools.h lane_tools.cpp UAP.h utils.h nfa.h dfa.h a_dfa.h auto.h mem_tools.h
	$(CC) $(CFLAGS) lane_tools.cpp

clean:
	$(RM) $(OBJS) UAP
