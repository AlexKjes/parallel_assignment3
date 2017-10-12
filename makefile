omp = rpcomp.c rpcomp.h util.c util.h
ptr = rpcpthread.c rpcpthread.h util.c util.h



omp:
	cc -fopenmp -O3 $(omp) -std=gnu99 -o omp
