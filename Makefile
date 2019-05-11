
CC = mpicc #mpiicc 
OPT =
CFLAGS = -Wall -std=c99 $(OPT) -OPT:IEEE_arith=1  #-qopenmp
LDFLAGS = -Wall #-qopenmp
LDLIBS = $(LDFLAGS)

targets = benchmark-naive benchmark-optimized
objects = check.o benchmark.o stencil-naive.o stencil-optimized.o

.PHONY : default
default : all

.PHONY : all
all : clean $(targets)

benchmark-naive : check.o benchmark.o stencil-naive.o
	$(CC) -o $@ $^ $(LDLIBS)

benchmark-optimized : check.o benchmark.o stencil-optimized.o
	$(CC) -o $@ $^ $(LDLIBS)

%.o : %.c common.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(targets) $(objects)
