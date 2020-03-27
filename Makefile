# --> FORMAT 
# target: [dependencies]
#        <command>
#        <command 2>

# macros (variables)
CC=clang++
CFLAGS=-I ./lib -ggdb
LIBS=$(wildcard lib/*.h)

# rules
final: final/threaded.cpp $(LIBS)
	$(CC) -std=c++11 final/threaded.cpp -o final/threaded $(CFLAGS)

arguments: arguments/go.cpp lib/args.hpp
	$(CC) -std=c++11 arguments/go.cpp -o arguments/run $(CFLAGS)

example: parallel/example.cpp $(LIBS)
	$(CC) -std=c++11 parallel/example.cpp -o parallel/run
