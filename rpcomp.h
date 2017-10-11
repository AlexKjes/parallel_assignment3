
#ifndef OVING3_RPCOMP_H
#define OVING3_RPCOMP_H


#define WHITE   0
#define ROCK    1
#define PAPER   2
#define SCISSOR 3

#define IMG_X 1024
#define IMG_Y 1024
#define N_CELLS (IMG_X*IMG_Y)

#define N_ITERATIONS 3000
#define N_SEEDS 100

typedef unsigned char uchar;

typedef struct {
    int color;
    int strength;
} cell;



cell* allocImg();
cell** allocRowPtrs(cell* img);
void seedPetri(cell* petri);

cell pick_neighbor(int x, int y, cell* image, int rnd);
cell next_cell(int x, int y, cell* image, int rnd);
int beats(cell me, cell other);

void cellToPixel(char* pixel_p, cell x);
void savebmp(char *name, char *buffer);

int imSoRandom(int *someValue, int *anotherValue);
#endif //OVING3_RPCOMP_H
