#ifndef _HUMAN_C_
#define _HUMAN_C_

#include "human.h"
#include <stdio.h>  /* printf(), scanf(), getchar() */


/* Function: human_move                                                       */
/*   Asks user for a column to put dist into.                                 */
/*   Function will repeat requests untill user types valid choice or standard */
/*   input encounters a failure.                                              */
/* Parameter(s):                                                              */
/*   board - board structure                                                  */
/* Returns:                                                                   */
/*   valid column index (zero-based) if successful, -1 if input encountered a */
/*   failure.                                                                 */
int human_move(conn4_state* board) {
    int ret;        /* Status of scanf() function call */
    int ch;
    int column = 0; /* Column chosen by user */

    do {
        printf("Choose column (1-%d): ", get_cols());
        ret = scanf("%d", &column);
        if (ret == 0) { /* User's input couldn't be converted to integer */
            printf("That is bad input. Please, enter an integer.\n");
            /* Clear last input line */
            do {
                ch = getchar();
            } while (ch != EOF && ch != '\n');
        } else if (ret == 1) {  /* User's input is successfully converted */
            if (column <= 0 || column > get_cols()) {
                printf("That is bad input. Please, enter an integer in "
                    "range from 1 to %d.\n", get_cols());
            } else if (get_height(board, column-1) == get_rows()) {
                printf("That is bad input. Please, choose column that is "
                    "not full.\n");
                column = 0;
            }
        } else {
            printf("Error: unexpected end of input. Cannot proceed.\n");
            column = 0;
        }
    } while (ret >= 0 && (column <= 0 || column > get_cols()));

    /* Transform index from one-based to zero-based. */
    return (column - 1);
}


#endif /* _HUMAN_C_ */
