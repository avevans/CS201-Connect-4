#ifndef _RATING_H_
#define _RATING_H_


#define COMPUTER_NAME   "Albert-AI"
#define FILENAME        "ratings.txt"


/* Asks user for name (nickname) until valid name is entered.                 */
char* choose_name(void);

/* Writes result of game to score table.                                      */
void save_result(const char* playerX, const char* playerO, char winner);

/* Loads score tables from file ("ratings.txt")                               */
void load_ratings(void);

/* Saves score table to file ("ratings.txt")                                  */
void save_ratings(void);


#endif /* _RATING_H_ */
