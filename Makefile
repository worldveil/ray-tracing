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
production: src/render.cpp $(LIBS)
	$(CC) -std=c++11 src/render.cpp -o tracer $(CFLAGS) $(OPT)

development: src/render.cpp $(LIBS)
	$(CC) -std=c++11 src/render.cpp -o tracer $(CFLAGS) $(DEBUGGING)
