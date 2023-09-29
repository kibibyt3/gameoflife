#define IO_KEY_PAUSE 'p'
#define IO_KEY_QUIT  'q'
#define IO_KEY_CRAWL '.'

typedef enum IOKeypress{
	IO_KEYPRESS_PAUSE = -3, /* Must be equal to the total amount of elements * -1 */
	IO_KEYPRESS_QUIT,
	IO_KEYPRESS_CRAWL
} IOKeypress;

extern int      io_screen_init(void);         /* Initializes the I/O module */
extern int      io_screen_end(void);          /* Terminates the I/O module */
extern square_t io_user_square_get(int, int); /* Returns user-chosen square */
extern int      io_game_cell_set(cell_t);     /* Sets a given cell on the screen */
