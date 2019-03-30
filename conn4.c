#ifndef _CONN4_C_
#define _CONN4_C_

#include "conn4.h"
#include <stdlib.h>     /* malloc() */
#include <string.h>     /* memset() */
#include <stdio.h>      /* printf() */



/* Size of char type in bits. This is the number of cells that can be stored  */
/* in one char.                                                               */
#define CHAR_SIZE       8
#define CHAR_SIZE_MASK  0x7

/* Total number of rows/columns stays constant during the game.               */
/* These variables are accessible only within this source file. From outside  */
/* one should use get_rows(), get_cols() and set_dimensions() functions.      */
static unsigned int COLS = DEFAULT_COLUMNS;
static unsigned int ROWS = DEFAULT_ROWS;
static unsigned int SIZE = DEFAULT_COLUMNS * DEFAULT_ROWS;

/* Number of chunks in array that store cells, 8 cells per chunk.             */
static unsigned int CHUNKS = (DEFAULT_COLUMNS * DEFAULT_ROWS + CHAR_SIZE - 1) /
                                CHAR_SIZE;

/* Macros: CELL_POS                                                           */
/*   Converts (column,row) pair of indices into single index of cell in       */
/*   flattened one-dimensional array of cells in column-major order.          */
#define CELL_POS(column,row)    ((column) * ROWS + (row))

/* Macros: CELL_IND                                                           */
/*   Obtains index of cell in one-dimensional array in column-major order     */
/*   under assumption that one chunk of array stores 8 cells.                 */
#define CELL_IND(column,row)    (CELL_POS(column,row) / CHAR_SIZE)

/* Macros: CELL_MASK                                                          */
/*   Obtains bit mask of cell that extract concerete bit from chunk of array  */
/*   that represents a cell.                                                  */
#define CELL_MASK(column,row)   (1 << (CELL_POS(column,row) & CHAR_SIZE_MASK))


/* Function: set_dimensions                                                   */
/*   Defines dimensions of game board.                                        */
/*   Note that changing dimensions invalidates all previously instantiated    */
/*   boards.                                                                  */
/* Parameter(s):                                                              */
/*   columns - horizontal dimension (number of columns) of game board         */
/*   rows    - vertical dimension (number of rows) of game board              */
void set_dimensions(unsigned int columns, unsigned int rows) {
    ROWS = rows;
    COLS = columns;
    SIZE = ROWS * COLS;
    CHUNKS = (SIZE + CHAR_SIZE - 1) / CHAR_SIZE;
    return;
}


/* Function: get_rows                                                         */
/* Returns:                                                                   */
/*   Vertical dimension (number of rows) of game board.                       */
unsigned int get_rows(void) {
    return ROWS;
}


/* Function: get_cols                                                         */
/* Returns:                                                                   */
/*   Horizontal dimension (number of columns) of game board.                  */
unsigned int get_cols(void) {
    return COLS;
}


/* Function: get_size                                                         */
/* Returns:                                                                   */
/*   Full size (number of cells) of game board.                               */
unsigned int get_size(void) {
    return SIZE;
}


/* Function: create_board                                                     */
/*   Creates an empty board.                                                  */
/* Returns:                                                                   */
/*   Empty board, or NULL if memory allocation failed.                        */
conn4_state* create_board(void) {
    conn4_state* board;
    board = malloc(sizeof(board));
    if (board != NULL) {
        board->info = malloc(CHUNKS + COLS);
        if (board->info != NULL) {
            board->moves = 0;
            memset(board->info, 0, CHUNKS + COLS);
        } else {
            free(board);
            board = NULL;
        }
    }
    return board;
}


/* Function: destruct_board                                                   */
/*   Destruct a board (release memory)                                        */
/* Parameter(s):                                                              */
/*   board - board structure                                                  */
void destruct_board(conn4_state* board) {
    if (board != NULL) {
        free(board->info);
        free(board);
    }
    return;
}


/* Function: print_horizontal_line                                            */
/*   Helper function that prints a horizontal separator between rows of board.*/
/*   Used in print_board() function.                                          */
void print_horizontal_line(void) {
    int col;
    for (col = 0; col < COLS; ++col) {
        printf("+---");
    }
    printf("+\n");
    return;
}


/* Function: print_board                                                      */
/*   Prints board in readable ASCII format.                                   */
/* Parameter(s):                                                              */
/*   board - board structure                                                  */
void print_board(conn4_state* board) {
    int row, col;

    /* Print header (column numbers) */
    printf("\n");
    for (int col = 0; col < get_cols(); ++col) {
    	printf("%3d ", col + 1);
    }
    printf("\n");

    print_horizontal_line();
    for (row = ROWS - 1; row >= 0; --row) {
        for (col = 0; col < COLS; ++col) {
            printf("| %c ", get_cell(board, col, row));
        }
        printf("|\n");
        print_horizontal_line();
    }
    return;
}


/* Function: get_cell                                                         */
/*   Gets type of disk at selected cell.                                      */
/*   Check for correctness of parameters isn't performed and should be done   */
/*   on side of caller.                                                       */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   column - column index (zero-based, starts from left side)                */
/* Returns:                                                                   */
/*   Character of disk at selected cell ('X', 'O' or ' ' (empty)).            */
char get_cell(conn4_state* board, unsigned int column, unsigned int row) {
    return (row >= board->info[CHUNKS+column]
            ? CELL_EMPTY
            : ((board->info[CELL_IND(column,row)] & CELL_MASK(column,row))
                ? CELL_X : CELL_O));
}


/* Function: set_cell                                                         */
/*   Places a disk of selected type on top of selected column (if it isn't    */
/*   full).                                                                   */
/*   Check for correctness of parameters isn't performed and should be done   */
/*   on side of caller.                                                       */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   column - column index (zero-based, starts from left side)                */
/*   disk   - character of disk at selected cell ('X' or 'O')                 */
/* Returns:                                                                   */
/*   1 if move is valid, 0 otherwise.                                         */
int set_cell(conn4_state* board, unsigned int column, char disk) {
    unsigned int height = get_height(board, column);
    if (height == ROWS) {
        return 0;   /* Cannot place disk on top of full column */
    }
    /* If disk is 'X', set 1 bit using corresponding bit mask */
    if (disk == CELL_X) {
        board->info[CELL_IND(column, height)] |= CELL_MASK(column, height);
    }
    /* If disk is 'O', set 0 bit; but bit is already 0, so do nothing. */
    /* Increase height of selected column and return success code */
    ++(board->info[CHUNKS+column]);
    ++(board->moves);
    return 1;
}


/* Function: unset_cell                                                       */
/*   Removes last disk dropped into selected column (if there is a disk).     */
/*   Check for correctness of parameters isn't performed and should be done   */
/*   on side of caller.                                                       */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   column - column index (zero-based, starts from left side)                */
void unset_cell(conn4_state* board, unsigned int column) {
    unsigned int height = get_height(board, column);
    /* Check if column is not empty */
    if (height > 0) {
        --(board->moves);
        board->info[CHUNKS+column] = (0xff & --height);
        /* Clear coresponding bit */
        board->info[CELL_IND(column, height)] &= ~CELL_MASK(column, height);
    }
    return;
}


/* Function: get_height                                                       */
/*   Gets height of stack of disks accumulated at selected column.            */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   column - column index (zero-based, starts from left side)                */
/* Returns:                                                                   */
/*   Height of stack of disks at selected column.                             */
int get_height(conn4_state* board, unsigned int column) {
    return board->info[CHUNKS+column];
}


/* Function: check_win_vert                                                   */
/*   Checks if player won by gathering 4 disks vertically.                    */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   column - column index of last move (zero-based, starts from left side)   */
/*   row    - row index of last move (zero-based, starts from bottom side)    */
/* Returns:                                                                   */
/*   1 if win, 0 otherwise.                                                   */
int check_win_vert(conn4_state* board, unsigned int column, unsigned int row) {
    unsigned int r;
    char disk = get_cell(board, column, row);   /* Type of disk of last move */

    /* Check if there is enough disks in column */
    if (row + 1 < COUNT_TO_WIN) {
        return 0;
    }
    /* Check if disks under last placed one are of the same type */
    for (r = row + 1 - COUNT_TO_WIN; r < row; ++r) {
        if (get_cell(board, column, r) != disk) {
            return 0;
        }
    }
    /* WIN!!! */
    return 1;
}


/* Function: check_win_hor                                                    */
/*   Checks if player won by gathering 4 disks horizontally.                  */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   column - column index of last move (zero-based, starts from left side)   */
/*   row    - row index of last move (zero-based, starts from bottom side)    */
/* Returns:                                                                   */
/*   1 if win, 0 otherwise.                                                   */
int check_win_hor(conn4_state* board, unsigned int column, unsigned int row) {
    int c;
    int count = 1;  /* Counter of neighboring disks of same kind in same row */
    char disk = get_cell(board, column, row);   /* Type of disk of last move */
    
    /* Count neighboring disks of same type to the left */
    for (c = column - 1; c >= 0; --c) {
        if (get_cell(board, c, row) == disk) {
            ++count;
        } else {
            break;
        }
    }
    /* Count neighboring disks of same type to the right */
    for (c = column + 1; c < COLS; ++c) {
        if (get_cell(board, c, row) == disk) {
            ++count;
        } else {
            break;
        }
    }
    /* WIN if at least 4 disks */
    return (count >= COUNT_TO_WIN);
}


/* Function: check_win_diag1                                                  */
/*   Checks if player won by gathering 4 disks on a "rising" diagonal (the    */
/*   one going from left bottom corner to right top corner).                  */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   column - column index of last move (zero-based, starts from left side)   */
/*   row    - row index of last move (zero-based, starts from bottom side)    */
/* Returns:                                                                   */
/*   1 if win, 0 otherwise.                                                   */
int check_win_diag1(conn4_state* board, unsigned int column, unsigned int row) {
    int c, r;
    int count = 1;  /* Counter of neighboring disks of same kind on diagonal */
    char disk = get_cell(board, column, row);   /* Type of disk of last move */
    
    /* Count neighboring disks of same type to the left and down */
    for (c = column - 1, r = row - 1; c >= 0 && r >= 0; --c, --r) {
        if (get_cell(board, c, r) == disk) {
            ++count;
        } else {
            break;
        }
    }
    /* Count neighboring disks of same type to the right and up */
    for (c = column + 1, r = row + 1; c < COLS && r < ROWS; ++c, ++r) {
        if (get_cell(board, c, r) == disk) {
            ++count;
        } else {
            break;
        }
    }
    /* WIN if at least 4 disks */
    return (count >= COUNT_TO_WIN);
}


/* Function: check_win_diag2                                                  */
/*   Checks if player won by gathering 4 disks on a "falling" diagonal (the   */
/*   one going from left top corner to right bottom corner).                  */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   column - column index of last move (zero-based, starts from left side)   */
/*   row    - row index of last move (zero-based, starts from bottom side)    */
/* Returns:                                                                   */
/*   1 if win, 0 otherwise.                                                   */
int check_win_diag2(conn4_state* board, unsigned int column, unsigned int row) {
    int c, r;
    int count = 1;  /* Counter of neighboring disks of same kind on diagonal */
    char disk = get_cell(board, column, row);   /* Type of disk of last move */
    
    /* Count neighboring disks of same type to the left and up */
    for (c = column - 1, r = row + 1; c >= 0 && r < ROWS; --c, ++r) {
        if (get_cell(board, c, r) == disk) {
            ++count;
        } else {
            break;
        }
    }
    /* Count neighboring disks of same type to the right and down */
    for (c = column + 1, r = row - 1; c < COLS && r >= 0; ++c, --r) {
        if (get_cell(board, c, r) == disk) {
            ++count;
        } else {
            break;
        }
    }
    /* WIN if at least 4 disks */
    return (count >= COUNT_TO_WIN);
}


/* Function: check_win                                                        */
/*   Checks if player won by gathering 4 disks in any available direction.    */
/* Parameter(s):                                                              */
/*   board  - board structure                                                 */
/*   column - column index of last move (zero-based, starts from left side)   */
/* Returns:                                                                   */
/*   1 if win, 0 otherwise.                                                   */
int check_win(conn4_state* board, unsigned int column) {
    /* Determine row coordinate of the last move */
    unsigned int row = get_height(board, column) - 1;

    /* Check four directions: vertical, horizontal and two diagonals */
    if (check_win_vert(board, column, row)) {
        return 1;   /* WIN!!! */
    }
    if (check_win_hor(board, column, row)) {
        return 1;   /* WIN!!! */
    }
    if (check_win_diag1(board, column, row)) {
        return 1;   /* WIN!!! */
    }
    if (check_win_diag2(board, column, row)) {
        return 1;   /* WIN!!! */
    }

    return 0;       /* No win */
}


#endif /* _CONN4_C_ */
