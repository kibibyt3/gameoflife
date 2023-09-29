#include <ncurses.h>
#include <unistd.h>
#include <stdbool.h> /* Required for life.h */

#include "life.h"
#include "io.h"
#include "preset.h"

void main_draw(void);

int main(int argc, char** argv){
	io_screen_init();
	linit(LINES, COLS);
	preset_init();

	bool     ispaused = true;
	bool     currently_alive;
	square_t user_square = {LINES / 2, COLS / 2};
	cell_t   user_cell;
	chtype   ch;
	int      c;
	
	/* Deal with presets called in argv */
	while ((c = getopt (argc, argv, "rhv")) != -1){
		preset_set(c);
	}
	
	main_draw();
	refresh();

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
	while ((ch = getch()) != IO_KEY_QUIT){
		if (((IOKeypress)(user_square.x)) == IO_KEYPRESS_QUIT){
			break;
		}

		if (ch == IO_KEY_PAUSE){
			ispaused = true;
		}
		while (ispaused){

			user_square = io_user_square_get(user_square.y, user_square.x);

			if (((IOKeypress)(user_square.x)) == IO_KEYPRESS_PAUSE){
				ispaused = false;
				user_square.y = LINES / 2;
				user_square.x = COLS / 2;
				break;
			} else if (((IOKeypress)(user_square.x)) == IO_KEYPRESS_QUIT){
				break;
			} else if (((IOKeypress)(user_square.x)) == IO_KEYPRESS_CRAWL){
				user_square.y = LINES / 2;
				user_square.x = COLS / 2;
				break; /* This because ispaused will not be unset */
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

		main_draw();

		refresh();	
	
	}
	io_screen_end();
	lexit();
	return 0;
}

void
main_draw() {
	square_t active_square;
	cell_t   cell;

	/* Draw every cell to the screen */
	for (active_square.y = 0; active_square.y < LINES; active_square.y++){
		for (active_square.x = 0; active_square.x < COLS; active_square.x++){
			cell.square.y = active_square.y;
			cell.square.x = active_square.x;
			cell.isalive = squareisalive(active_square);
			io_game_cell_set(cell);
		}
	}
}
