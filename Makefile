# --> FORMAT 
# target: [dependencies]
#        <command>
#        <command 2>

# macros (variables)
CC=clang++
CFLAGS=-I ./lib
DEBUGGING=-ggdb
OPT=-O3
LIBS=$(wildcard lib/*.h)

# rules
production: final/threaded.cpp $(LIBS)
	$(CC) -std=c++11 final/threaded.cpp -o tracer $(CFLAGS) $(OPT)

development: final/threaded.cpp $(LIBS)
	$(CC) -std=c++11 final/threaded.cpp -o tracer $(CFLAGS) $(DEBUGGING)

arguments: arguments/go.cpp lib/args.hpp
	$(CC) -std=c++11 arguments/go.cpp -o arguments/run $(CFLAGS)

example: parallel/example.cpp $(LIBS)
	$(CC) -std=c++11 parallel/example.cpp -o parallel/run
