/*
 * life.h
 * Interface for the module responsible for the game of life proper. Call
 * linit() to initialize the game. Call this before any other function and
 * ensure lexit is called whenever the game is finished. Use the structure
 * square to interface with the module. Call ltick() to make a tick pass in
 * the game. To interface with specific squares, call squareisalive() or
 * setsquare(). Depends on stdbool.h.
 */

#define L_FAILURE -1
#define L_SUCCESS  0

#define ALIVE true
#define DEAD false

/*
 * Struct to handle in-game squares from an line/column perspective
 */
typedef struct square_t {
	int y;
	int x;	
} square_t;

/*
 * Struct to talk about a given cell, meaning a square with an associated
 * alive/dead value.
 */
typedef struct cell_t {
	square_t square;
	bool     isalive;
}cell_t;

extern int  linit(int, int);           /* Initializes the game */
extern int  lexit(void);        			 /* Exits the game */
extern int  ltick(void);               /* Causes an in-game tick to pass */
extern bool squareisalive(square_t);   /* Returns whether a square is alive */
extern int  setsquare(square_t, bool); /* Sets an in-game square to a value */
