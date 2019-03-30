#ifndef _COMPUTER_C_
#define _COMPUTER_C_

#include "computer.h"
#include <time.h>       /* time(), clock() */
#include <stdlib.h>     /* srand(), rand() */
#include <stdio.h>


/* Main points on scale of position estimation                                */
#define WIN     +1.0
#define DRAW     0.0
#define LOSS    -1.0

/* Macros: Determines player whose turn is now                                */
#define CURR_PLAYER(board)  ((board)->moves % 2 == 0 ? CELL_X : CELL_O)

/* Macros: Determines player whose turn is next                               */
#define NEXT_PLAYER(board)  ((board)->moves % 2 != 0 ? CELL_X : CELL_O)

/* Macros: Determines player whose turn was earlier                           */
#define PREV_PLAYER(board)  NEXT_PLAYER(board)

/* Macros: Determines greater value of two                                    */
#define MAX(a,b)    ((a) > (b) ? (a) : (b))


/* Function: count_win_moves                                                  */
/*   Searches for moves that yields winning alignment of disks immediately.   */
/* Parameter(s):                                                              */
/*   board - board structure                                                  */
/*   disk  - type of disk to put                                              */
/*   move  - where one of winning move will be written (if such exists)       */
/* Returns:                                                                   */
/*   Number of different moves that immediately yield winning alignment.      */
int count_win_moves(conn4_state* board, char disk, int* move) {
    int count = 0;
    int column;

    for (column = 0; column < get_cols(); ++column) {
        if (set_cell(board, column, disk)) {
            if (check_win(board, column)) {
                ++count;
                *move = column; /* Send winning move to caller */
            }
            unset_cell(board, column);  /* Backtract */
        }
    }

    return count;
}


/* Function: quick_win                                                        */
/*   Evaluates position on board and checks if it is possible to win in just  */
/*   one or two moves. It is assumed that current player placed disk in       */
/*   selected column.                                                         */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   column - column of last move                                             */
/*   est    - where result of game (WIN/DRAW/LOSS) will be written            */
/* Returns:                                                                   */
/*   1 if game is over or will be finished in one or two moves, 0 otherwise.  */
/*   Estimation is returned via pointer only if result of check is positive.  */
int quick_win(conn4_state* board, unsigned int column, float* est) {
    int count;      /* Number of winning moves */
    int move;       /* Winning move */
    int ret = 1;    /* Result of this function */
    char disk = PREV_PLAYER(board);     /* Disk type of current player */
    char opponent = CURR_PLAYER(board); /* Disk type of opponent */

    if (check_win(board, column)) {
        *est = WIN; /* Winning alignment already on board! */
    } else if (board->moves == get_size()) {
        *est = DRAW;/* Board is full and noone won */
    } else if (count_win_moves(board, opponent, &move) > 0) {
        *est = LOSS;/* Opponent wins in next move */
    } else if ((count = count_win_moves(board, disk, &move)) > 1) {
        *est = WIN; /* Player has at least two different winning moves, hence */
                    /* opponent can't block all winning moves - it's a win!   */
    } else if (count == 1) {
        ret = 0;
        /* Assume that opponent will block player's only winning move.        */
        /* Check if this will immediately let current player win anyway.      */
        set_cell(board, move, opponent);
        if (set_cell(board, move, disk)) {
            if (check_win(board, move)) {
                ret = 1;
                *est = WIN; /* Opponent looses even if he/she tries to block  */
            }
            unset_cell(board, move);
        }
        unset_cell(board, move);
    } else {
        ret = 0;    /* Can't find win alignment whithin next two moves. */
    }

    return ret;
}


/* Function: win_alignment_right                                              */
/*   Checks if selected cell completes winning alignment to the right if      */
/*   player's disk is put at this cell.                                       */
/* Parameter(s):                                                              */
/*   board - board structure                                                  */
/*   col   - selected column                                                  */
/*   row   - selected row                                                     */
/*   disk  - player's disk type                                               */
/* Returns:                                                                   */
/*   1 if cell completes winning alignment to the right, 0 otherwise.         */
unsigned int win_alignment_right(conn4_state* board, unsigned int col,
        unsigned int row, char disk) {
    unsigned int count;

    if (col + COUNT_TO_WIN > get_cols()) {
        return 0;   /* Not enough space to right to make winning alignment */
    }
    /* Check horizontal alignment */
    for (count = 1; count < COUNT_TO_WIN; ++count) {
        if (get_cell(board, col + count, row) != disk) {
            break;
        }
    }
    if (count == COUNT_TO_WIN) {
        return 1;   /* Horizontal win alignment of disks */
    }
    /* Check diagonal alignment (diagonal rises up) */
    if (row + COUNT_TO_WIN <= get_rows()) {
        for (count = 1; count < COUNT_TO_WIN; ++count) {
            if (get_cell(board, col + count, row + count) != disk) {
                break;
            }
        }
        if (count == COUNT_TO_WIN) {
            return 1;   /* Rising diagonal win alignment of disks */
        }
    }
    /* Check diagonal alignment (diagonal falls down) */
    if (row + 1 >= COUNT_TO_WIN) {
        for (count = 1; count < COUNT_TO_WIN; ++count) {
            if (get_cell(board, col + count, row - count) != disk) {
                break;
            }
        }
        if (count == COUNT_TO_WIN) {
            return 1;   /* Rising diagonal win alignment of disks */
        }
    }
    /* Chosen cell doesn't make any winning alignment to the right */
    return 0;
}


/* Function: win_alignment_left                                               */
/*   Checks if selected cell completes winning alignment to the left if       */
/*   player's disk is put at this cell.                                       */
/* Parameter(s):                                                              */
/*   board - board structure                                                  */
/*   col   - selected column                                                  */
/*   row   - selected row                                                     */
/*   disk  - player's disk type                                               */
/* Returns:                                                                   */
/*   1 if cell completes winning alignment to the left, 0 otherwise .         */
unsigned int win_alignment_left(conn4_state* board, unsigned int col,
        unsigned int row, char disk) {
    unsigned int count;

    if (col + 1 < COUNT_TO_WIN) {
        return 0;   /* Not enough space to left to make winning alignment */
    }
    /* Check horizontal alignment */
    for (count = 1; count < COUNT_TO_WIN; ++count) {
        if (get_cell(board, col - count, row) != disk) {
            break;
        }
    }
    if (count == COUNT_TO_WIN) {
        return 1;   /* Horizontal win alignment of disks */
    }
    /* Check diagonal alignment (diagonal rises up) */
    if (row + COUNT_TO_WIN <= get_rows()) {
        for (count = 1; count < COUNT_TO_WIN; ++count) {
            if (get_cell(board, col - count, row + count) != disk) {
                break;
            }
        }
        if (count == COUNT_TO_WIN) {
            return 1;   /* Rising diagonal win alignment of disks */
        }
    }
    /* Check diagonal alignment (diagonal falls down) */
    if (row + 1 >= COUNT_TO_WIN) {
        for (count = 1; count < COUNT_TO_WIN; ++count) {
            if (get_cell(board, col - count, row - count) != disk) {
                break;
            }
        }
        if (count == COUNT_TO_WIN) {
            return 1;   /* Rising diagonal win alignment of disks */
        }
    }
    /* Chosen cell doesn't make any winning alignment to the left */
    return 0;
}

/* Function: count_open_pos                                                   */
/*   Counts how many cells on the board can complete winning alignment except */
/*   of those accessible immediately.                                         */
/* Parameter(s):                                                              */
/*   board - board structure                                                  
/*   disk  - player's disk type                                               */
/* Returns:                                                                   */
/*   1 if cell completes winning alignment to the right, 0 otherwise.         */
unsigned int count_open_pos(conn4_state* board, char disk) {
    unsigned int count = 0U;
    unsigned int column, row;

    for (column = 0; column < get_cols(); ++column) {
        for (row = get_height(board, column) + 1; row < get_rows(); ++row) {
            count += (win_alignment_left(board, column, row, disk)
                    | win_alignment_right(board, column, row, disk));
        }
    }

    return count;
}


/* Function: eval                                                             */
/*   Evaluates position on board from point of view of player who made last   */
/*   move.                                                                    */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   column - column of last move                                             */
/* Returns:                                                                   */
/*   Estimation of "probability" on the scale -1...+1.                        */
/*   -1 if player will definitely loose, +1 if player will definitely win,    */
/*   0 if the game will probably end as a draw. Positive value in range       */
/*   0...+1 indicate level of confidence in victory, and negative values in   */
/*   range -1...0 indicate level of confidence in opponents victory.          */
float eval(conn4_state* board, unsigned int column) {
    float est;              /* Estimation of chances to win */
    int opens, opponent;    /* Number of open positions of player and opponent*/

    /* Find out if any player wins in two nearest moves */
    if (quick_win(board, column, &est)) {
        return est;
    }
    /* Evaluate open position for both players */
    opens = count_open_pos(board, CURR_PLAYER(board));
    opponent = count_open_pos(board, NEXT_PLAYER(board));
    /* Return the following value: difference between counts of open positions*/
    /* divided by total number of positions for two players plus 1.           */
    /* Such estimator is strictly between -1 and +1 (both ends exclusively)   */
    /* and is symmetrical.                                                    */
    return (float)(opens - opponent) / (opens + opponent + 1);
}


/* Function: force_move                                                       */
/*   Checks if player must make certain move (in order to win immediately or) */
/*   block opponent form winning).                                            */
/* Parameter(s):                                                              */
/*   board - board structure                                                  */
/*   move  - where necessary move will be stored (if such exists)             */
/* Returns:                                                                   */
/*   1 if certain move is required, 0 otherwise.                              */
int force_move(conn4_state* board, int* move) {
    if (count_win_moves(board, CURR_PLAYER(board), move) > 0) {
        return 1;   /* Can win immediately! */
    }
    if (count_win_moves(board, NEXT_PLAYER(board), move) > 0) {
        return 1;   /* Must block opponent */
    }
    return 0;
}


/* Function: eval_rec                                                         */
/*   Evaluates position on board from point of view of player who made last   */
/*   move. Evaluation is performed based on recursive traversing the graph of */
/*   the game (depth-first-search) till limited depth. It is assumed that     */
/*   current player placed disk in selected column.                           */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   column - column of last move                                             */
/*   depth  - maximal depth of search                                         */
/* Returns:                                                                   */
/*   Estimation of "probability" on the scale -1...+1.                        */
/*   -1 if player will definitely loose, +1 if player will definitely win,    */
/*   0 if the game will probably end as a draw. Positive value in range       */
/*   0...+1 indicate level of confidence in victory, and negative values in   */
/*   range -1...0 indicate level of confidence in opponents victory.          */
float eval_rec(conn4_state* board, unsigned int column, int depth) {
    unsigned int i;
    unsigned int move;  /* Move for opponent */
    float est;          /* Estimation for opponent's move */
    float best = LOSS;  /* The best estimation for opponent's move */
    char disk = CURR_PLAYER(board);

    /* Recursion stop condition - maximal depth reached. */
    if (depth <= 0) {
        return eval(board, column);
    }

    if (force_move(board, &move)) {
        /* If certain move is necessary now, there's no need to expand other  */
        /* moves on search tree.                                              */
        set_cell(board, move, disk);
        if (check_win(board, move)) {
            best = WIN;
        } else {
            /* Can go deeper in the search tree without counting this level   */
            /* because this level had no branching.                           */
            best = eval_rec(board, move, depth);
        }
        unset_cell(board, move);    /* Backtrack, restore board's state */
    } else {
        for (i = 0; i < get_cols(); ++i) {
            /* Select moves in order from central column to corners. Central  */
            /* columns have higher priority as they give more opportunities.  */
            /* The following formula for values i=0,1,2,3... gives columns    */
            /* n/2, n/2-1, n/2+1, n/2-2, ..., where n is number of columns.   */
            move = get_cols() / 2 - (i + 1) / 2 * ((i % 2) * 2 - 1);
            /* Try to make move in selected column */
            if (set_cell(board, move, disk)) {
                if (!quick_win(board, move, &est)) {
                    /* Evaluate board from the opponent's point of view */
                    est = eval_rec(board, move, depth - 1);
                    best = MAX(best, est);
                }   /* Else estimation is computed automatically */
                best = MAX(best, est);      /* Keep best score of opponent */
                unset_cell(board, move);    /* Backtrack */
            }
        }
    }

    /* Assuming that opponent uses the same strategy. Hence choose the best   */
    /* estimation of his/her/its chances. Their chances to win is opposite to */
    /* chances to win of current player, hence we'll return opposite value.   */
    return -best;
}


/* Function: computer_move_rec                                                */
/*   Searches for the best available move based on recursive traversing the   */
/*   graph of the game (depth-first-search) till limited depth.               */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   depth  - maximal depth of search                                         */
/*   forced - flag to indicate the caller that returned move is necessary and */
/*            further search is redundant                                     */
/* Returns:                                                                   */
/*   Index of bets found move.                                                */
int computer_move_rec(conn4_state* board, unsigned int depth, int* forced) {
    int i, c;
    float est;
    int column;             /* Best move found so far */
    float best = LOSS - 1;  /* Score of the best move found so far */

    /* Deduce type of disk of the computer player */
    char disk = CURR_PLAYER(board);

    /* Check if there is a necessary move */
    if (force_move(board, &column)) {
        *forced = 1;
        return column;
    }

    for (i = 0; i < get_cols(); ++i) {
        /* Select moves in order from central column to corners. Central      */
        /* columns have higher priority as they give more opportunities.      */
        /* The following formula for values i=0,1,2,3... gives columns        */
        /* n/2, n/2-1, n/2+1, n/2-2, ..., where n is number of columns.       */
        c = get_cols() / 2 - (i + 1) / 2 * ((i % 2) * 2 - 1);
        /* Try putting disk in selected column */
        if (set_cell(board, c, disk)) {
            /* Evaluate last move. If it's better than previous - update best */
            est = eval_rec(board, c, depth);
            if (est > best) {
                best = est;
                column = c;
            }
            unset_cell(board, c);   /* Backtrack */
        }
    }

    return column;
}




/* Function: computer_move                                                    */
/*   Computer's decision-making function.                                     */
/*   Currently function is written so that it tries to find obvious move      */
/*   (which yields immediate win or blocks opponent from winning), and if     */
/*   there is no obvious move, it selects the best move based on game graph   */
/*   traversal (depth-first-search) with incrementally increasing maximal     */
/*   depth of search until function used more than 1 second of computer time. */
/* Parameter(s):                                                              */
/*   board - board structure                                                  */
/* Returns:                                                                   */
/*   Index of chosen move.                                                    */
int computer_move(conn4_state* board) {
    clock_t t0 = clock();   /* Fix current computer time */

    unsigned int depth = 0; /* Set initial maximal depth */
    int column;
    int forced = 0;         /* Flag indicating that found move is necessary */

    do {
        column = computer_move_rec(board, depth++, &forced);
    } while (!forced && (clock() - t0) < CLOCKS_PER_SEC);

    /* Output selected column in one-based indexing */
    printf("Selected column: %d\n", column + 1);

    return column;
}


#endif /* _COMPUTER_C_ */
