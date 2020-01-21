#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define WIDTH 7
#define HEIGHT 6

typedef enum {NON, MATCHNUL, ORDI_GAGNE, HUMAIN_GAGNE } FinDePartie;

typedef struct EtatSt {

    int player;

    char board[WIDTH][HEIGHT];

} State;

State * init_state() {
    State * state = (State *)malloc(sizeof(State));

    int i,j;
    for (i=0; i< WIDTH; i++)
        for ( j=0; j<HEIGHT; j++)
            state->board[i][j] = ' ';

    return state;
}

void print_game(State * state) {

    int i,j;
    printf("   |");
    for ( j = 0; j < WIDTH; j++)
        printf(" %d |", j + 1);
    printf("\n");
    printf("----------------");
    printf("\n");

    for(i=0; i < HEIGHT; i++) {
        printf(" %d |", HEIGHT - i);
        for ( j = 0; j < WIDTH; j++)
            printf(" %c |", state->board[i][j]);
        printf("\n");
        printf("----------------");
        printf("\n");
    }
}



int main() {

    State * state = init_state();

    print_game(state);

    return 0;
}