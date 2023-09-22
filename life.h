/*
 * life.h
 * Interface for the module responsible for the game of life proper. Call
 * linit() to initialize the game. Call this before any other function and
 * ensure lexit is called whenever the game is finished. Use the structure
 * square to interface with the module. Call ltick() to make a tick pass in
 * the game. To interface with specific squares, call squareisalive() or
 * setsquare().
 */


typedef struct square_t square_t;

extern int linit(int, int);
extern int lexit();
extern int ltick();
extern bool squareisalive(square_t);
extern int setsquare(square_t, bool);
