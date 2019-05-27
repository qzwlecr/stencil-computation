CC = mpicc
SWCC = sw5cc
OPT =
CFLAGS = -O3 -I./include -Wall -std=gnu99 $(OPT) -OPT:IEEE_arith=1 -LNO:simd=1
LDFLAGS = -Wall #-Wl,--whole-archive,-wrap,athread_init,-wrap,__expt_handler,-wrap,__real_athread_spawn /home/export/online1/swmore/release/lib/libspc.a -Wl,--no-whole-archive
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
	$(SWCC) -slave -c $(CFLAGS) $< -o $@ -lm -msimd

.PHONY: clean
clean:
	rm -rf $(targets) $(objects)
