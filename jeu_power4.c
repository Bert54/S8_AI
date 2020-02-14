#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#define WIDTH 7
#define HEIGHT 6

#define TIME 3		// temps de calcul pour un coup avec MCTS (en secondes)

#define MAX_CHILDREN WIDTH

#define OTHER_PLAYER(i) (1-(i))

typedef enum {NON, MATCHNUL, ORDI_GAGNE, HUMAIN_GAGNE } FinDePartie;

typedef struct EtatSt {

    int player;

    char board[HEIGHT][WIDTH];

} State;

typedef struct {

    int column;

} Move;

typedef struct NoeudSt {

    int player; // joueur qui a joué pour arriver ici
    Move * move;   // coup joué par ce joueur pour arriver ici

    State * state; // etat du jeu

    struct NoeudSt * parent;
    struct NoeudSt * children[MAX_CHILDREN]; // liste d'enfants : chaque enfant correspond à un coup possible
    int nb_children;	// nb d'enfants présents dans la liste

    // POUR MCTS:
    int nb_victories;
    int nb_simus;
    int max_node;

} Node;

State * copy_state(State * src) {
    State * state = (State *)malloc(sizeof(State));

    state->player = src->player;

    int i,j;
    for (i=0; i< HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            state->board[i][j] = src->board[i][j];
        }
    }

    return state;
}

State * init_state() {
    State * state = (State *)malloc(sizeof(State));

    int i,j;
    for (i=0; i< HEIGHT; i++)
        for ( j=0; j<WIDTH; j++)
            state->board[i][j] = ' ';

    return state;
}

void print_game(State * state) {

    int i,j;
    printf("   |");
    for ( j = 0; j < WIDTH; j++)
        printf(" %d |", j);
    printf("\n");
    printf("--------------------------------");
    printf("\n");

    for(i=0; i < HEIGHT; i++) {
        printf(" %d |", HEIGHT - i - 1);
        for ( j = 0; j < WIDTH; j++)
            printf(" %c |", state->board[i][j]);
        printf("\n");
        printf("--------------------------------");
        printf("\n");
    }
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

Move ** possible_moves(State *state) {

    Move ** moves = (Move **) malloc((1+MAX_CHILDREN) * sizeof(Move *) );

    int k = 0;
    int i;
    int j;
    for(j = 0 ; j < WIDTH ; j++) {
        i = HEIGHT-1;
        while (state->board[i][j] != ' ' && i >= 0) {
            --i;
        }
        if (i >= 0) {
            moves[k] = new_move(j);
            k++;
        }
    }

    moves[k] = NULL;

    return moves;
}

Node * new_node(Node * parent, Move * coup, int max_node ) {
    Node * noeud = (Node *)malloc(sizeof(Node));

    if ( parent != NULL && coup != NULL ) {
        noeud->state = copy_state(parent->state);
        play_move( noeud->state, coup );
        noeud->move = coup;
        noeud->player = OTHER_PLAYER(parent->player);
    }
    else {
        noeud->state = NULL;
        noeud->move = NULL;
        noeud->player = 0;
    }
    noeud->parent = parent;
    noeud->nb_children = 0;

    // POUR MCTS:
    noeud->nb_victories = 1;
    noeud->nb_simus = 1;

    if (max_node) {
        noeud->max_node = 1;
    }
    else {
        noeud->max_node = 0;
    }


    return noeud;
}

Node * add_child(Node * parent, Move * coup, int max_node) {
    Node * enfant = new_node(parent, coup, max_node ) ;
    parent->children[parent->nb_children] = enfant;
    parent->children[parent->nb_children]->parent = parent;
    parent->nb_children++;
    return enfant;
}

void free_node(Node * noeud) {
    if ( noeud->state != NULL)
        free (noeud->state);

    while ( noeud->nb_children > 0 ) {
        free_node(noeud->children[noeud->nb_children-1]);
        noeud->nb_children --;
    }
    if ( noeud->move != NULL)
        free(noeud->move);

    free(noeud);
}

FinDePartie end_test(State * state) {

    int i, j, k, n = 0;
    for (i = 0 ; i < HEIGHT ; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (state->board[i][j] != ' ') {
                n++;

                // lignes
                k = 0;
                while (k < 4 && j + k < WIDTH && state->board[i][j + k] == state->board[i][j]) {
                    k ++;
                }
                if (k >= 4)
                    return state->board[i][j] == 'O' ? ORDI_GAGNE : HUMAIN_GAGNE;

                // colonnes
                k=0;
                while ( k < 4 && i + k >= 0 && state->board[i - k][j] == state->board[i][j] )
                    k++;
                if ( k >= 4 )
                    return state->board[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

                // diagonales
                k=0;
                while ( k < 4 && j+k < WIDTH && i+k < HEIGHT && state->board[i+k][j+k] == state->board[i][j] )
                    k++;
                if ( k >= 4 )
                    return state->board[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

                k=0;
                while ( k < 4 && i+k < HEIGHT && j-k >= 0 && state->board[i+k][j-k] == state->board[i][j] )
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

int is_win(Node * node) {
    if (end_test(node->state) == ORDI_GAGNE) {
        return 1;
    }
    return 0;
}

int is_lose(Node * node) {
    if (end_test(node->state) == HUMAIN_GAGNE) {
        return 1;
    }
    return 0;
}

Node * ai_select_node_with_best_b_value(Node * node) {
    // si il y a encore des enfants, c'est qu'on n'est pas dans une feuille, on continue donc à chercher
    if (node->nb_children > 0) {
        double b_values[node->nb_children];
        int i;
        // si un noeud enfant fait gagner l'ordi, on retourne ce noeud
        for (i = 0; i < node->nb_children; i++) {
            if (is_win(node->children[i])) {
                return node->children[i];
            }
        }
        // calcul des b-valeurs
        for (i = 0; i < node->nb_children; i++) {
            //b_values[i] = rand() % 1000;
            float mu;
            mu = (float)node->children[i]->nb_victories / (float)node->children[i]->nb_simus;
            if (node->children[i]->max_node == 0) {
                b_values[i] = mu * -1 + sqrt(2) * sqrt(log(node->nb_simus) / node->children[i]->nb_simus);
            }
            else {
                b_values[i] = mu + sqrt(2) * sqrt(log(node->nb_simus) / node->children[i]->nb_simus);
            }
        }
        // sélection du noeud avec la meilleure b-valeur
        Node *best_node = node->children[0];
        int best_node_ind = 0;
        for (i = 0; i < node->nb_children; i++) {
            //printf("%f\n", b_values[i]);
            if (b_values[i] > b_values[best_node_ind]) {
                best_node = node->children[i];
                best_node_ind = i;
            }
        }
        return ai_select_node_with_best_b_value(best_node);
    }
    return node;
}

Node * ai_expand_node_and_choose_new_child(Node * node) {
    Move **moves;
    moves = possible_moves(node->state);
    int k = 0;
    while (moves[k] != NULL) {
        if (node->max_node == 1) {
            add_child(node, moves[k], 0);
        }
        else {
            add_child(node, moves[k], 1);
        }
        node->children[k]->parent = node;
        k++;
    }
    return node->children[rand() % k];
}

int ai_simulate_game_playout(Node * node) {

    Node * copy = new_node(NULL, NULL, 1);
    copy->state = copy_state(node->state);
    copy->max_node = node->max_node;
    FinDePartie fdp;
    do {
        Move **moves;
        moves = possible_moves(copy->state);
        int move_amount = 0;
        while (moves[move_amount] != NULL) {
            move_amount++;
        }
        Move * selected_move = moves[rand() % move_amount];
        play_move(copy->state, selected_move);
        if (copy->max_node == 1) {
            copy->max_node = 0;
        }
        else {
            copy->max_node = 1;
        }
        fdp = end_test(copy->state);
    } while (fdp == NON);
    free_node(copy);
    if (fdp == ORDI_GAGNE) {
        return 1;
    }
    return 0;
}

void update_nodes_with_reward(Node * node, int reward) {
    while (node->parent != NULL) {
        node->nb_simus ++;
        node->nb_victories += reward;
        node = node->parent;
    }
    node->nb_simus ++;
    node->nb_victories += reward;
}

void ai_play_mcts(State * state, int maxtime) {

    clock_t tic, toc;
    tic = clock();
    int temps;

    Move **moves;
    Move *best_move;

    // Créer l'arbre de recherche
    Node *root = new_node(NULL, NULL, 1);
    root->state = copy_state(state);

    // créer les premiers noeuds:
    moves = possible_moves(root->state);
    int k = 0;
    while (moves[k] != NULL) {
        add_child(root, moves[k], 0);
        k++;
    }



    //best_move = moves[rand() % k]; // choix aléatoire

    int iter = 0;

    root->nb_victories = 1;
    root->nb_simus = 1;
    root->max_node = 0;
    Node *n = NULL;
    do {
         n = ai_select_node_with_best_b_value(root);
         if (!is_win(n)) {
             n = ai_expand_node_and_choose_new_child(n);
             int reward = ai_simulate_game_playout(n);
             update_nodes_with_reward(n, reward);
         }
            // à compléter par l'algorithme MCTS-UCT...
        toc = clock();
        temps = (int)( ((double) (toc - tic)) / CLOCKS_PER_SEC );
        iter ++;
    } while ( temps < maxtime );

    /* fin de l'algorithme  */

    int i = 0;
    double best_ratio = 0;
    int best_child = 0;
    for (i = 0 ; i < root->nb_children ; i++) {
        float ratio = 0;
        if (is_win(root->children[i])) {
            best_ratio = 999999999;
            best_child = i;
        }
        else {
            int j;
            int lose = 0;
            for (j = 0 ; j < root->children[i]->nb_children ; j++) {
                if (is_lose(root->children[i]->children[j])) {
                    lose = 1;
                }
            }
            if (lose) {
                best_ratio = 99999999;
                best_child = i;
            }
            else {
                if (root->children[i]->nb_simus > 0) {
                    ratio = (float) root->children[i]->nb_victories / (float) root->children[i]->nb_simus;
                }
                if (ratio > best_ratio) {
                    best_child = i;
                    best_ratio = ratio;
                }
            }
        }
    }
    best_move = root->children[best_child]->move;

    // Jouer le meilleur premier coup
    play_move(state, best_move);

    // Penser à libérer la mémoire :
    free_node(root);
    free(moves);
}

int main() {

    State *state = init_state();
    Move *move;
    FinDePartie end = NON;
    printf("Who starts? (0 : human, 1 : computer)");
    scanf("%d", &(state->player) );
    do {
        printf("\n");
        print_game(state);
        if ( state->player == 0 ) {
            // tour de l'humain

            do {
                move = ask_move();
            } while (!play_move(state, move));

        }
        else {
            // tour de l'Ordinateur

            ai_play_mcts(state, TIME);

        }
        end = end_test(state);
    } while ( end == NON ) ;

    printf("\n");
    print_game(state);

    if ( end == ORDI_GAGNE )
        printf( "** The computer won **\n");
    else if ( end == MATCHNUL )
        printf(" It's a tie!  \n");
    else
        printf( "** CONGRATULATIONS, the computer lost **\n");

    return 0;
}