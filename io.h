#define PAUSE -1
#define QUIT  -2
#define PAUSE_BUTTON 'p'
#define QUIT_BUTTON 'q'

extern int      io_screen_init();         /* Initializes the I/O module */
extern int      io_screen_end();          /* Terminates the I/O module */
extern square_t io_user_square_get();     /* Returns user-chosen square */
extern int      io_game_cell_set(cell_t); /* Sets a given cell on the screen */
