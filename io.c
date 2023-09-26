#include <assert.h>
#include <stddef.h>
#include <ncurses.h>
#include <stdbool.h>

#include "life.h" /* Required for access to square_t & cell_t */
#include "io.h"

#define CURSOR_MOVEMENT_FACTOR   10
#define DELAY_DS                 1
#define CHARACTER_ALIVE         '@'
#define CHARACTER_DEAD          ' '

bool screen_isactive = false;

/*
 * Initializes the screen. Returns L_SUCCESS on a successful initialization;
 * L_FAILURE otherwise. May abort. Sets screen_isactive to true.
 */
int
io_screen_init() {

	/* Abort if initialization isn't successful */
	assert(screen_isactive == false);
	assert(initscr() != NULL);
	assert(halfdelay(DELAY_DS) != ERR);
	assert(noecho() != ERR);
	assert(curs_set(0) != ERR);
	assert(keypad(stdscr, TRUE) != ERR);

	screen_isactive = true;
	return L_SUCCESS;
}

/*
 * Terminates the screen. Returns L_SUCCESS on a successful termination;
 * L_FAILURE otherwise. May abort. Sets screen_isactive to false.
 */
int
io_screen_end() {
	assert(screen_isactive == true);
	assert(endwin() != ERR);

	screen_isactive = false; 
	return L_SUCCESS;
}

/*
 * Acquires a square given by the user via the arrow keys or WASD. Pressing
 * enter or the spacebar causes the function to return the square the user
 * is on, or a square with its x component equal to a given IOKeypress if its
 * corresponding IO_KEY_* is pressed. May abort.
 */
square_t
io_user_square_get(int y, int x) {
	assert(screen_isactive == true);
	
	assert(curs_set(1) != ERR); /* Make user cursor visible */
	assert(cbreak() != ERR);    /* Temporarily leave halfdelay mode */
	
	int ch;
	bool key_special_flag = false;
	IOKeypress key;

	move(y, x);
	while ((ch = getch()) != KEY_ENTER){
		if (ch == ' '){break;}
		
		/* Handle special keys */
		else if (ch == IO_KEY_PAUSE){key_special_flag = true; key = IO_KEYPRESS_PAUSE; break;}
		else if (ch == IO_KEY_QUIT) {key_special_flag = true; key = IO_KEYPRESS_QUIT;  break;}
		else if (ch == IO_KEY_CRAWL){key_special_flag = true; key = IO_KEYPRESS_CRAWL; break;}
		
		/* Cursor movement by one space */
		switch (ch) {
		case 'w': /* FALLTHROUGH */
		case KEY_UP:
			if (y != 0){
				y--;
			}
			break;
		case 's': /* FALLTHROUGH */
		case KEY_DOWN:
			if (y != LINES - 1){
				y++;
			}
			break;
		case 'a': /* FALLTHROUGH */
		case KEY_LEFT:
			if (x != 0){
				x--;
			}
			break;
		case 'd': /* FALLTHROUGH */
		case KEY_RIGHT:
			if (x != COLS - 1){
				x++;
			}
			break;

	/* Cursor movement by a factor of CURSOR_MOVEMENT_FACTOR */
		case 'h':
			if (x >= CURSOR_MOVEMENT_FACTOR){
				x -= CURSOR_MOVEMENT_FACTOR;
			} else{
				x = 0;
			}
			break;

		case 'l':
			if (x < COLS - CURSOR_MOVEMENT_FACTOR){
				x += CURSOR_MOVEMENT_FACTOR;
			} else{
				x = COLS - 1;
			}
			break;

		case 'k':
			if (y >= CURSOR_MOVEMENT_FACTOR){
				y -= CURSOR_MOVEMENT_FACTOR;
			} else{
				y = 0;
			}
			break;

		case 'j':
			if (y < LINES - CURSOR_MOVEMENT_FACTOR){
				y += CURSOR_MOVEMENT_FACTOR;
			} else{
				y = LINES - 1;
			}
			break;

		}
		move(y, x);	
	}
	square_t user_square = {y, x};
	

	/* Return to how we were before */
	assert(curs_set(0) != ERR);
	assert(halfdelay(DELAY_DS) != ERR);

	if (key_special_flag == true){
		user_square.x = (int)key;
	}

	return user_square;
}

/*
 * Adds the cell given in argument cell to the screen. Uses CHARACTER_ALIVE
 * if the cell is alive; uses CHARACTER_DEAD otherwise. May abort.
 */
int
io_game_cell_set(cell_t cell) {
	assert(screen_isactive == true);
	if (cell.isalive) {
		assert((mvaddch(cell.square.y, cell.square.x, (chtype) CHARACTER_ALIVE) != ERR) || ((cell.square.y == LINES - 1) && (cell.square.x == COLS - 1))); /* mvaddch() returns an error for a failure to line-wrap. */
	}
	else {
		assert((mvaddch(cell.square.y, cell.square.x, (chtype) CHARACTER_DEAD) != ERR) || ((cell.square.y == LINES - 1) && (cell.square.x == COLS - 1)));
	}
	return L_SUCCESS;
}
