#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h> /* Required for life.h */

#include "preset.h"
#include "life.h"

#define RANDOM_DENOMINATOR 2

static int preset_isinit = false;

static void preset_random(int);
static void preset_line_horizontal(void);
static void preset_line_vertical(void);

void
preset_init(){
	assert(preset_isinit == false);
	preset_isinit = true;
	srand((unsigned int) time(0));
}

void
preset_set(int ch){
	assert(preset_isinit == true);
	switch (ch){
	case 'r':
		preset_random(RANDOM_DENOMINATOR);
		break;
	case 'h':
		preset_line_horizontal();
		break;
	case 'v':
		preset_line_vertical();
		break;
	}
}

void
preset_random(int denominator){
	assert(preset_isinit == true);
	square_t square_parse;
	bool isdead;

	for(square_parse.y = 0; square_parse.y < LINES; square_parse.y++){
		for(square_parse.x = 0; square_parse.x < COLS; square_parse.x++){
			isdead = (bool) (rand() % denominator);
			if (isdead) {
				assert(setsquare(square_parse, false) != L_FAILURE);
			}
			else {
				assert(setsquare(square_parse, true) != L_FAILURE);
			}
		}
	}
}

void
preset_line_horizontal(){
	assert(preset_isinit == true);
	square_t square_parse;
	square_parse.y = LINES / 2;

	for (square_parse.x = 0; square_parse.x < COLS; square_parse.x++){
		assert(setsquare(square_parse, true) != L_FAILURE);
	}
}

void
preset_line_vertical(){
	assert(preset_isinit == true);
	square_t square_parse;
	square_parse.x = COLS / 2;

	for (square_parse.y = 0; square_parse.y < LINES; square_parse.y++){
		assert(setsquare(square_parse, true) != L_FAILURE);
	}
}
