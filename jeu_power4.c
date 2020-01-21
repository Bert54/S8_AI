#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define WIDTH 7
#define HEIGHT 6

#define OTHER_PLAYER(i) (1-(i))

typedef enum {NON, MATCHNUL, ORDI_GAGNE, HUMAIN_GAGNE } FinDePartie;

typedef struct EtatSt {

    int player;

    char board[WIDTH][HEIGHT];

} State;

typedef struct {

    int column;

} Move;


State * init_state() {
    State * state = (State *)malloc(sizeof(State));

    int i,j;
    for (i=0; i< WIDTH; i++)
        for ( j=0; j<HEIGHT; j++)
            state->board[i][j] = ' ';

    return state;
}

Move * new_move(int i) {
    Move * move = (Move *)malloc(sizeof(Move));
    move->column = i;
    return move;
}

Move * ask_move() {

    int i;
    printf(" Which column? ") ;
    scanf("%d",&i);

    return new_move(i);
}

int play_move( State * state, Move * move ) {

    int i = HEIGHT-1;
    while(state->board[i][move->column] != ' ' && i >= 0) {
        i--;
    }
    if (i < 0) {
        return 0;
    }
    else {
        state->board[i][move->column] = state->player ? 'O' : 'X';
        printf("%d\n", i);
        printf("%d\n", move->column);
        // à l'autre joueur de jouer
        state->player = OTHER_PLAYER(state->player);

        return 1;
    }
}

void print_game(State * state) {

    int i,j;
    printf("   |");
    for ( j = 0; j < WIDTH; j++)
        printf(" %d |", j);
    printf("\n");
    printf("----------------");
    printf("\n");

    for(i=0; i < HEIGHT; i++) {
        printf(" %d |", HEIGHT - i - 1);
        for ( j = 0; j < WIDTH; j++)
            printf(" %c |", state->board[i][j]);
        printf("\n");
        printf("----------------");
        printf("\n");
    }
}



int main() {

    State *state = init_state();
    Move *move;
    FinDePartie end = NON;
    do {
        printf("\n");
        print_game(state);
        do {
            move = ask_move();
        } while (!play_move(state, move));
    }while ( end == NON ) ;
    return 0;
}