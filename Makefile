CC = mpicc
OPT =
CFLAGS = -Wall -std=c99 $(OPT) -OPT:IEEE_arith=1
LDFLAGS = -Wall
LDLIBS = $(LDFLAGS)

targets = benchmark-naive benchmark-optimized
objects = check.o benchmark.o stencil-naive.o stencil-optimized.o stencil-slave.o

.PHONY : default
default : all

.PHONY : all
all : clean $(targets)

benchmark-naive : check.o benchmark.o stencil-naive.o
	$(CC) -o $@ $^ $(LDLIBS)

benchmark-optimized : check.o benchmark.o stencil-optimized.o stencil-slave.o
	$(CC) -hybrid -o $@ $^ $(LDLIBS)

check.o : check.c common.h
	$(CC) -c $(CFLAGS) $< -o $@

benchmark.o : benchmark.c common.h
	$(CC) -c $(CFLAGS) $< -o $@

stencil-optimized.o : stencil-optimized.c common.h
	$(CC) -host -c $(CFLAGS) $< -o $@

stencil-naive.o : stencil-naive.c common.h
	$(CC) -c $(CFLAGS) $< -o $@

stencil-slave.o : stencil-slave.c common.h
	$(CC) -slave -c $(CFLAGS) $< -o $@ -lm

.PHONY: clean
clean:
	rm -rf $(targets) $(objects)
