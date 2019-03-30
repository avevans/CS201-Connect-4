#ifndef _CONN4_C_
#define _CONN4_C_

#include "conn4.h"
#include "player.h"
#include "human.h"
#include "computer.h"
#include "rating.h"
#include <stdlib.h>     /* malloc() */
#include <stdio.h>      /* printf() */
#include <string.h>     /* strcmp() */
#include <unistd.h>     /* sleep()  */


#define HUMAN_VS_HUMAN      1
#define HUMAN_VS_COMPUTER   2
#define COMPUTER_VS_HUMAN   3


/* Function: init                                                             */
/*   Initializes parameters of board and loads ratings.                       */
void init(void) {
    int rows = 0, columns =0;

    printf("Choose dimensions of the game.\n");
    printf("Warning: dimensions higher than %dx%d will have unweildy screen "
        "handling.\n", MAX_ROWS, MAX_COLUMNS);

    do {
        printf("    Rows (default %d):    ", DEFAULT_ROWS);
        scanf("%d", &rows);
        if (rows < MIN_ROWS) {
            printf("    Out of range. Must be at least %d\n", MIN_ROWS);
        }
    } while (rows < MIN_ROWS);

    do {
        printf("    Columns (default %d): ", DEFAULT_COLUMNS);
        scanf("%d", &columns);
        if (columns < MIN_COLUMNS) {
            printf("    Out of range. Must be at least %d\n", MIN_COLUMNS);
        }
    } while (columns < MIN_COLUMNS);

    set_dimensions(columns, rows);
    load_ratings();

    return;
}


/* Function: menu                                                             */
/*   Greets user and lets him/her enter all required parameters.              */
/* Parameter(s):                                                              */
/*   players - two players to initialize                                      */
void menu(player_t* players) {
    int mode = 0;

    printf("\n   *** Welcome to \"CONNECT FOUR\"! ***\n");   sleep(2);

    printf("\n      Written by: Andrekious Evans\n");       
    printf("\n          Date: March 29, 2019\n");          
    printf("\n           Class: CS-201-001\n");             
    printf("\n     Instructor: Dr. Monica Anderson\n\n");   sleep(2);
    printf("\n                 LOADING\n\n"); sleep(4);
    init();

    /* Let user choose mode */
    printf("\n\nChoose game mode:\n");
    printf("1. Human (X) vs Human (O)\n");
    printf("2. Human (X) vs Computer (O)\n");
    printf("3. Human (O) vs Computer (X)\n");
    do {
        printf("    Your choice: ");
        scanf("%d", &mode);
    } while (mode != HUMAN_VS_HUMAN && mode != HUMAN_VS_COMPUTER &&
            mode != COMPUTER_VS_HUMAN);

    /* Initialize players according to selected mode */
    switch (mode) {
        case HUMAN_VS_HUMAN:
            printf("Player X (1st move) set-up.\n");
            players[0].get_move = human_move;
            players[0].name = choose_name();
            printf("Player O (2nd move) set-up.\n");
            players[1].get_move = human_move;
            do {
                players[1].name = choose_name();
            } while (strcmp(players[1].name, players[0].name) == 0);
            break;
        case HUMAN_VS_COMPUTER:
            printf("Player X (1st move) set-up.\n");
            players[0].get_move = human_move;
            players[0].name = choose_name();
            players[1].get_move = computer_move;
            players[1].name = COMPUTER_NAME;
            break;
        case COMPUTER_VS_HUMAN:
            players[0].get_move = computer_move;
            players[0].name = COMPUTER_NAME;
            printf("Player O (2nd move) set-up.\n");
            players[1].get_move = human_move;
            players[1].name = choose_name();
    }

    /* First player uses X disks, second - O disks */
    players[0].disk = CELL_X;
    players[1].disk = CELL_O;

    return;
}



/* Function: main                                                             */
/*   Starts the program. Initializes parameters, loads ratings from external  */
/*   file, creates empty board and lets human play agains other human or      */
/*   computer. Result is saved back to external file.                         */
/* Parameter(s): ignored                                                      */
int main(int argc, char* argv[]) {
    int column;                 /* Column selected by player */
    conn4_state* board = NULL;  /* Game board */
    int turn = 0;               /* Parity of current turn number */
    int victory = 0;            /* Flag indicating win condition */

    player_t players[2];

    /* Set-up. Choose dimensions, mode and user name(s). */
    menu(players);

    /* Initialize game board */
    board = create_board();
    printf("\nGame starts now...\n\n");
    print_board(board);

    while (board->moves < get_size()) {
        printf("\n\nTurn of player %s (%c).\n\n",
            players[turn].name, players[turn].disk);

        column = players[turn].get_move(board);
        if (!set_cell(board, column, players[turn].disk)) {
            printf("Unexpected error! Need to review set_cell() and/or "
                "human_move() and/or computer_move() functions.\n");
            destruct_board(board);
            return EXIT_FAILURE;
        }
        /* Print updated board after player's move */
        printf("\n\n");
        print_board(board);
        /* Check for win condition */
        if (check_win(board, column)) {
            victory = 1;
            printf("\n\nPlayer %s (%c) wins!\n\n\nwritten by: Andrekious Evans\n\n",
                players[turn].name, players[turn].disk);
            /* Update ratings */
            save_result(players[0].name, players[1].name, players[turn].disk);
            break;
        }
        /* Advance to next move and another player */
        turn = (board->moves % 2);
    }

    /* Check if game is a tie */
    if (!victory) {
        printf("\n\nGame is over and noone won.\n\n\nwritten by: Andrekious Evans\n\n");
        /* Update ratings (draw column for both players) */
        save_result(players[0].name, players[1].name, 0);
    }

    /* Finalize */
    destruct_board(board);
    save_ratings();

    return EXIT_SUCCESS;
}


#endif /* _CONN4_C_ */
