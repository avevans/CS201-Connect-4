#ifndef _CONN4_H_
#define _CONN4_H_

/* Default dimensions of Connect Four game board                              */
#define DEFAULT_COLUMNS 7
#define DEFAULT_ROWS    6

/* Minimal dimensions of Connect Four game board                              */
#define MIN_COLUMNS     4
#define MIN_ROWS        4
/* Maximal dimensions of Connect Four game board for optimal User Interface   */
#define MAX_COLUMNS     40
#define MAX_ROWS        40

/* Number of disks in a row to win                                            */
#define COUNT_TO_WIN    4

/* Cell types */
#define CELL_EMPTY  ' '
#define CELL_X      'X'
#define CELL_O      'O'


typedef struct conn4_struct {
    unsigned int moves; /* Moves made on this board */
    unsigned char* info;/* Information about positions of disks. Each         */
                        /* cell is represented by one bit: 0 for 'O' disk and */
                        /* 1 for 'X' disk. However, empty cell is represented */
                        /* by 0 bit as well; to resolve which 0 bit refers to */
                        /* empty cell and which to 'O' disk, we also store    */
                        /* height of each column - then 0 bits under this     */
                        /* height are 'O' disks and above this height - empty.*/
                        /* Cells are stored in one-dimensional array 8 cells  */
                        /* per chunk of array, and heights of columns are     */
                        /* stored in same array after all cells.              */
} conn4_state;

/* Set rows/columns dimensions of game board.                                 */
void set_dimensions(unsigned int columns, unsigned int rows);

/* Get vertical dimension (number of rows) of game board.                     */
unsigned int get_rows(void);

/* Get horizontal dimension (number of columns) of game board.                */
unsigned int get_cols(void);

/* Get full size of game board (number of cells).                             */
unsigned int get_size(void);

/* Create an empty board.                                                     */
conn4_state* create_board(void);

/* Destruct a board (release memory).                                         */
void destruct_board(conn4_state* board);

/* Prints board in readable ASCII format.                                     */
void print_board(conn4_state* board);

/* Gets type of disk at selected cell.                                        */
char get_cell(conn4_state* board, unsigned int column, unsigned int row);

/* Places a disk of selected type on top of selected column (if it isn't full)*/
int set_cell(conn4_state* board, unsigned int column, char disk);

/* Removes last disk dropped into selected column.                            */
void unset_cell(conn4_state* board, unsigned int column);

/* Gets height of stack of disks accumulated at selected column.              */
int get_height(conn4_state* board, unsigned int column);

/* Checks if the last move brings a victory.                                  */
int check_win(conn4_state* board, unsigned int column);


#endif /* _CONN4_H_ */
