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

    int i = WIDTH-2;
    while(state->board[i][move->column] != ' ' && i >= 0) {
        i--;
    }
    if (i >= WIDTH) {
        return 0;
    }
    else {
        state->board[i][move->column] = state->player ? 'O' : 'X';

        // à l'autre joueur de jouer
        //state->player = AUTRE_JOUEUR(etat->joueur);

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

    State * state = init_state();
    Move * move;
    print_game(state);
    do {
        move = ask_move();
    } while ( !play_move(state, move) );
    print_game(state);
    do {
        move = ask_move();
    } while ( !play_move(state, move) );
    print_game(state);
    do {
        move = ask_move();
    } while ( !play_move(state, move) );
    print_game(state);
    return 0;
}