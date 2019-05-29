
CC = mpicc #mpiicc 
SWCC = swacc

OPT =  -LNO:simd=1 -O3
SWOPT = -SCFlags -HCFlags -ver,5.421-sw-gy -SCFlags -ver,5.421-sw-gy -O3 -LNO:simd_report=2 -ldmAnalyse -dumpcommand mk
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
	$(SWCC) -o $@ $^ $(LDLIBS)
stencil-optimized.o : stencil-optimized.c
	swacc -c stencil-optimized.c -O3 -ldmAnalyse -dumpcommand mk
%.o : %.c common.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(targets) $(objects)
