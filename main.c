#include <ncurses.h>

#include "life.h"
#include "io.h"


int main(void){
	io_screen_init();
	linit(LINES, COLS);

	bool     ispaused = true;
	bool     currently_alive;
	square_t user_square = {LINES / 2, COLS / 2};
	square_t active_square;
	cell_t   user_cell;
	cell_t   cell;
	chtype   ch;
	
	/*
	 *  .----<--------------<----------------<---------------<-----------------<---------------.
	 *  |                                                                                      |
	 *  '-> Get character -> pause or quit as requested .-> pass a tick -> update screen state -'
	 *                         |                        |
	 *                         |                        |
	 *                         |                        '------<--------.
	 *                         |                                        |
	 *                         '-> get arbitrary amounts of user input -'
	 */
	while ((ch = getch()) != QUIT_BUTTON){
		if (user_square.x == QUIT){
			break;
		}
		if (ch == PAUSE_BUTTON){
			ispaused = true;
		}
		while (ispaused){
			user_square = io_user_square_get(user_square.y, user_square.x);
			if (user_square.x == PAUSE){
				ispaused = false;
				user_square.y = LINES / 2;
				user_square.x = COLS / 2;
				break;
			}
			if (user_square.x == QUIT){
				break;
			}
			else{
				currently_alive = squareisalive(user_square);
				setsquare(user_square, !currently_alive);
				user_cell.square = user_square;
				user_cell.isalive = !currently_alive;
				io_game_cell_set(user_cell);
			}
		}
		
		ltick();
		
		for (active_square.y = 0; active_square.y < LINES; active_square.y++){
			for (active_square.x = 0; active_square.x < COLS; active_square.x++){
				cell.square.y = active_square.y;
				cell.square.x = active_square.x;
				cell.isalive = squareisalive(active_square);
				io_game_cell_set(cell);
			}
		}
		
		refresh();
	
	}
	io_screen_end();
	lexit();
	return 0;
}
