#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#define WIDTH 7
#define HEIGHT 7

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

    char input[10];
    int input_integer, length, i;
    int inputIsNumber;
    do {
        inputIsNumber = 1;
        printf(" Which column? ");
        scanf("%s", &input);
        length = strlen(input);
        for (i=0;i<length; i++) {
            if (!isdigit(input[i])) {
                inputIsNumber = 0;
            }
        }
        sscanf(input, "%d", &input_integer);
    } while (!inputIsNumber || input_integer < 0 || input_integer >= WIDTH);
    return new_move(input_integer);
}

int play_move( State * state, Move * move ) {

    int i = HEIGHT-1;
    while(state->board[i][move->column] != ' ' && i >= 0) {
        --i;
    }
    if (i < 0) {
        return 0;
    }
    else {
        state->board[i][move->column] = state->player ? 'O' : 'X';
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

FinDePartie end_test(State * state) {

    int i, j, k, n = 0;
    for (i = 0 ; i < WIDTH ; i++) {
        for (j = 0 ; j < HEIGHT ; j++) {
            if ( state->board[i][j] != ' ') {
                n++;

                // lignes
                k=0;
                while ( k < 4 && i+k < WIDTH && state->board[i+k][j] == state->board[i][j] )
                    k++;
                if ( k >= 4 )
                    return state->board[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

                // colonnes
                k=0;
                while ( k < 4 && j+k < HEIGHT && state->board[i][j-k] == state->board[i][j] )
                    k++;
                if ( k >= 4 )
                    return state->board[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

                // diagonales
                k=0;
                while ( k < 4 && i+k < WIDTH && j+k < HEIGHT && state->board[i+k][j+k] == state->board[i][j] )
                    k++;
                if ( k >= 4 )
                    return state->board[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

                k=0;
                while ( k < 4 && i+k < WIDTH && j-k >= 0 && state->board[i+k][j-k] == state->board[i][j] )
                    k++;
                if ( k >= 4 )
                    return state->board[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
                }
            }
        }

    if ( n >= WIDTH * HEIGHT ) {
        return MATCHNUL;
    }

    return NON;

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
        end = end_test(state);
    } while ( end == NON ) ;

    printf("\n");
    print_game(state);

    if ( end == ORDI_GAGNE )
        printf( "** L'ordinateur a gagné **\n");
    else if ( end == MATCHNUL )
        printf(" Match nul !  \n");
    else
        printf( "** BRAVO, l'ordinateur a perdu  **\n");

    return 0;
}