#include <ncurses.h>

#include "life.h"
#include "io.h"


int main(void){
	io_screen_init();
	linit(LINES, COLS);

	bool paused = false;
	bool currently_alive;
	square_t user_square;
	square_t active_square;
	cell_t cell;
	chtype ch;
	
	while ((ch = getch()) != KEY_F(1)){
		if (ch == 'p'){
			while ((ch = getch()) != 'q'){
				user_square = io_user_square_get();
				currently_alive = squareisalive(user_square);
				setsquare(user_square, !currently_alive);	
			}
		}
		for (active_square.y = 0; active_square.y < LINES; active_square.y++){
			for (active_square.x = 0; active_square.x < COLS; active_square.x++){
				cell.square.y = active_square.y;
				cell.square.x = active_square.x;
				cell.isalive = squareisalive(active_square);
				io_game_cell_set(cell);
			}
		}
		ltick();
	}
}
