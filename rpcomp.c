#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <malloc.h>
#include "rpcomp.h"
#include <time.h>
#include <limits.h>
int tId;
int* rnd;

int main(int argc, char** argv) {
    cell* petris[] = {allocImg(),
                       allocImg()};
    char* img = calloc(IMG_Y*IMG_X*3, sizeof(char));


    int nThreads = atoi(argv[1]);
    omp_set_num_threads(nThreads);
    for (int i=0;i<nThreads;i++){
        rnd[i] = rand();
    }
    seedPetri(petris[0]);

    /// debugvars

    struct timespec start, end;
    double elapsedTime;
    /// debugvars

    clock_gettime(CLOCK_MONOTONIC, &start);
    #pragma omp parallel private(tId, rnd) shared(petris, img)
    {

        tId = omp_get_thread_num();


        rnd = rand();
        int seed;
        #pragma omp critical
        {
            srand(tId);
            seed = rand();
            seed = (seed * 0x5DEECE66DL + 0xBL) & 0xFFFFFFFFFFFFL;
        }
        for (int i = 0; i < N_ITERATIONS; i++) {
            #pragma omp for schedule(static)
            for (int j = 0; j < N_CELLS; j++) {
                petris[(i + 1) % 2][j] = next_cell(j % IMG_X, j / IMG_X, petris[i % 2], (seed % 8) + 8*(seed < 8));
            }

        }

        #pragma omp for schedule(static)
        for (int i=0;i<N_CELLS;i++) {
            cellToPixel(&img[i*3], petris[N_ITERATIONS % 2][i]);
        }


    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsedTime = (end.tv_sec-start.tv_sec);
    elapsedTime += (end.tv_nsec-start.tv_nsec)/1000000000.0;
    savebmp("RPS_omp.bmp", img);



    printf("Time spent: %lfs", elapsedTime);

    return 0;
}

cell* allocImg(){
    return calloc(IMG_X*IMG_Y, sizeof(cell));
}

cell** allocRowPtrs(cell* img){
    cell** row_p = malloc(IMG_Y*sizeof(cell*));
    for (int i=0;i<IMG_Y;i++){
        row_p[i] = &img[i*IMG_X];
    }
    return row_p;
}

void seedPetri(cell* petri){
    for (int i=0;i<N_SEEDS;i++){
        int r_pos = rand()%N_CELLS;
        int r_color = (rand()%3)+1;
        petri[r_pos].color = r_color;
        petri[r_pos].strength = 1;
    }
}

int beats(cell me, cell other){
    return
            (((me.color == SCISSOR) && (other.color == PAPER)) ||
             ((me.color == PAPER) && (other.color == ROCK))    ||
             ((me.color == ROCK) && (other.color == SCISSOR))  ||
             (me.color == other.color)) && ((me.color == SCISSOR) || (me.color == PAPER) || (me.color == ROCK));
}

cell next_cell(int x, int y, cell* image, int rnd){

    cell neighbor_cell = pick_neighbor(x, y, image, rnd);
    cell my_cell = image[y*IMG_X+x];
    if(neighbor_cell.color == WHITE){
        return my_cell;
    }
    cell next_cell = my_cell;

    if(my_cell.color == WHITE){
        next_cell.strength = 1;
        next_cell.color = neighbor_cell.color;
        return next_cell;
    }
    else {
        if(beats(my_cell, neighbor_cell)){
            next_cell.strength++;
        }
        else{
            next_cell.strength--;
        }
    }

    if(next_cell.strength == 0){
        next_cell.color = neighbor_cell.color;
        next_cell.strength = 1;
    }

    if(next_cell.strength > 4){
        next_cell.strength = 4;
    }

    return next_cell;
}

cell pick_neighbor(int x, int y, cell*  image, int pms){

    cell border;
    border.color = 0;
    border.strength = 0;
    if(x==0 || y == 0 || x==IMG_X-1 | y==IMG_Y-1){
        return border;
    }


    int chosen = imSoRandom(&tId, &rnd[tId]);
   // printf("%d\n", chosen);
    if(chosen >= 4){ chosen++; } // a cell cant choose itself
    int c_x = chosen % 3;
    int c_y = chosen / 3;

    return image[(y + c_y - 1)*IMG_X+(x + c_x - 1)];

}

void cellToPixel(char* p_p, cell x){
    if (x.color==0){
        p_p[0] = 255;
        p_p[1] = 255;
        p_p[2] = 255;
    }
    else {
        p_p[x.color-1] = 255;
    }
}

/* save 24-bits bmp file, buffer must be in bmp format: upside-down */
void savebmp(char *name,char *buffer) {
    int x = IMG_X;
    int y = IMG_Y;
    FILE *f=fopen(name,"wb");
    if(!f) {
        printf("Error writing image to disk.\n");
        return;
    }
    unsigned int size=x*y*3+54;
    uchar header[54]={'B','M',size&255,(size>>8)&255,(size>>16)&255,size>>24,0,
                      0,0,0,54,0,0,0,40,0,0,0,x&255,x>>8,0,0,y&255,y>>8,0,0,1,0,24,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    fwrite(header,1,54,f);
    fwrite(buffer,1,N_CELLS*3,f);
    fclose(f);
}


int imSoRandom(int *someValue, int *anotherValue){
    (*anotherValue) = ((*someValue+1) * *anotherValue + 25643) % INT_MAX;
    return (*anotherValue%8);
}