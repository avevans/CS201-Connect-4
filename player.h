#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "conn4.h"


/* Player (either human or computer) is presented with current state of game  */
/* board and asked to make a move.                                            */
/* Player's move is returned as a column (zero-based indexing) to which       */
/* he/she/it decided to put their disk.                                       */
/* Below is the definition for a type of functions that behave as player.     */
typedef int (*get_move_func)(conn4_state*);

/* Player type besides of decision-making function has name and playing disk. */
typedef struct {
	get_move_func	get_move;	/* Function that makes moves */
	char 			disk;		/* Disk type of player, either 'X' or 'O' */
	char* 			name;		/* Name of player */
} player_t;


#endif /* _PLAYER_H_ */
