#ifndef _RATING_C_
#define _RATING_C_

#include "conn4.h"
#include "rating.h"
#include <stdio.h>  /* printf(), fprintf(), scnaf(), fscanf(), FILE */
#include <string.h> /* strcmp() */
#include <stdlib.h> /* malloc() */


#define MAX_NAME_LENGTH 32


/* Use rentry structure */
typedef struct {
    char* name;
    unsigned int wins;
    unsigned int losses;
    unsigned int draws;
} user_t;


/* List of rated users. */
user_t* rated = NULL;
/* Number of rates users. */
unsigned int countRated = 0;
/* Maximal number of rated users. */
unsigned int maxRated = 0;


/* Function: find_user                                                        */
/*   Searches for a user given his/hr name.                                   */
/* Parameter(s):                                                              */
/*   name - name of searched user (case sensitive)                            */
/* Returns:                                                                   */
/*   Index of user in score table if found, or -1 if not found.               */
int find_user(const char* name) {
    int i;

    for (i = 0; i < countRated; ++i) {
        if (strcmp(name, rated[i].name) == 0) {
            return i;
        }
    }

    return -1;
}


/* Function: add_user                                                         */
/*   Adds user entry to score table.                                          */
/* Parameter(s):                                                              */
/*   user - new user entry                                                    */
void add_user(user_t* user) {
    user_t* old;
    if (countRated == maxRated) {
        if (maxRated == 0) {
            maxRated = 1;
            rated = malloc(sizeof(*rated));
        } else {
            old = rated;
            rated = malloc(2 * maxRated * sizeof(*rated));
            for (countRated = 0; countRated < maxRated; ++countRated) {
                rated[countRated] = old[countRated];
            }
            free(old);
            maxRated *= 2;
        }
    }
    rated[countRated++] = *user;
    return;
}


/* Function: create_user                                                      */
/*   Creates a new user entry for a given name.                               */
/* Returns:                                                                   */
/*   New user entry.                                                          */
user_t* create_user(const char* name) {
    user_t* user = malloc(sizeof(*user));

    user->name = malloc(MAX_NAME_LENGTH+1);
    strcpy(user->name, name);

    user->wins = 0;
    user->losses = 0;
    user->draws = 0;

    return user;
}


/* Function: choose_name                                                      */
/*   Asks user for a name and creates a new entry in score table if name is   */
/*   new. After successful choosing, printf current rating of user.           */
/* Returns:                                                                   */
/*   Name selected by user. This string mustn't be freed by the caller.       */
/*   It will be automatically freed after calling save_raings() function.     */
char* choose_name(void) {
    char name[MAX_NAME_LENGTH+1];
    int ok = 0;     /* Status of name creation */
    int i;
    user_t* user;

    /* Ask for a name until valid name is entered */
    do {
        printf("Pick a name: ");
        scanf("%32s", name);
        if ((ok = strcmp(name, COMPUTER_NAME)) == 0) {
            printf("This name is reserved for computer AI.\n");
        }
    } while (!ok);

    /* If user is new - create new entry in score table */
    if ((i = find_user(name)) < 0) {
        i = countRated;
        add_user(create_user(name));
    }

    /* Print current rating */
    printf("\nYour current rating:\n   %d wins, %d losses, %d draws.\n\n",
            rated[i].wins, rated[i].losses, rated[i].draws);

    return rated[i].name;
}


/* Function: save_result                                                      */
/*   Saves result of a game.                                                  */
/* Parameter(s):                                                              */
/*   playerX - name of player who played with disks of X type                 */
/*   playerO - name of player who playe with disks of O type                  */
/*   winner  - type of disks of winner (X or O) or any other symbol if a draw */
void save_result(const char* playerX, const char* playerO, char winner) {
    /* Find users */
    int iX = find_user(playerX);
    int iO = find_user(playerO);

    /* Udate ratings of two players */
    if (winner == CELL_X) {
        rated[iX].wins++;
        rated[iO].losses++;
    } else if (winner == CELL_O) {
        rated[iO].wins++;
        rated[iX].losses++;
    } else {
        rated[iX].draws++;
        rated[iO].draws++;
    }

    return;
}


/* Function: load_ratings                                                     */
/*   Loads ratings of registered users from file. Call this function once at  */
/*   start up.                                                                */
void load_ratings(void) {
    char name[MAX_NAME_LENGTH+1];
    unsigned int wins, losses, draws;
    user_t* user;
    int i;

    /* Read users from file */
    FILE* file = fopen(FILENAME, "r");
    if (file != NULL) {
        while (!feof(file)) {
            if (fscanf(file, "%32s %u %u %u", name,
                    &wins, &losses, &draws) == 4) {
                user = create_user(name);
                user->wins = wins;
                user->losses = losses;
                user->draws = draws;
                add_user(user);
            } else {
                break;
            }
        }
        fclose(file);
    }

    /* Make sure that computer is rated as well */
    if (find_user(COMPUTER_NAME) < 0) {
        add_user(create_user(COMPUTER_NAME));
    }

    /* Output all ratings */
    printf("  *** Score table ***\n");
    for (i = 0; i < countRated; ++i) {
        printf("    %s: %d wins, %d losses, %d draws\n",
                rated[i].name, rated[i].wins, rated[i].losses, rated[i].draws);
    }

    return;
}


/* Function: save_ratings                                                     */
/*   Saves ratings of registered users to file. Call this function once at    */
/*   exit because it will remove all ratings and free memory.                 */
void save_ratings(void) {
    int i;
    /* Write users to file */
    FILE* file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("\nError: cannot save ratings.\n\n");
    } else {
        for (i = 0; i < countRated; ++i) {
            fprintf(file, "%s\t%u\t%u\t%u\n", rated[i].name, rated[i].wins,
                    rated[i].losses, rated[i].draws);
            free(rated[i].name);
        }
        fclose(file);

        /* Remove all ratings */
        free(rated);
        rated = NULL;
        countRated = maxRated = 0;
    }

    return;
}


#endif /* _RATING_C_ */
