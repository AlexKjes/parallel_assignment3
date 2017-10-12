#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "rpcomp.h"

int tId;
int rnd;

/**
 *  Ikke veldig random men........
 */

int main(int argc, char** argv) {
    cell* petris[] = {allocImg(),
                       allocImg()};
    char* img = calloc(IMG_Y*IMG_X*3, sizeof(char));


    int nThreads = atoi(argv[1]);
    omp_set_num_threads(nThreads);
    seedPetri(petris[0]);

    /// debugvars

    //struct timespec start, end;
    //double elapsedTime;
    /// debugvars

    //clock_gettime(CLOCK_MONOTONIC, &start);
    #pragma omp parallel private(tId, rnd) shared(petris, img)
    {

        tId = omp_get_thread_num();
        rnd = tId;


        for (int i = 0; i < N_ITERATIONS; i++) {
            rand_r((uint*)&rnd);

            #pragma omp for schedule(static)
            for (int j = 0; j < N_CELLS; j++) {
                petris[(i + 1) % 2][j] = next_cell(j % IMG_X, j / IMG_X, petris[i % 2], rnd<1);
            }

        }

        #pragma omp for schedule(static)
        for (int i=0;i<N_CELLS;i++) {
            cellToPixel(&img[i*3], petris[N_ITERATIONS % 2][N_CELLS-i+1]);
        }


    }

    //clock_gettime(CLOCK_MONOTONIC, &end);
    //elapsedTime = (end.tv_sec-start.tv_sec);
    //elapsedTime += (end.tv_nsec-start.tv_nsec)/1000000000.0;
    savebmp("RPS_omp.bmp", img);



    //printf("Time spent: %lfs", elapsedTime);

    return 0;
}

