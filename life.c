/*
 * Internal logic for the game of life. linit() must be called to initialize
 * the board before anything else.
 */

#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include <life.h>

#define VBYTE_SIZE  8  /* This value MAY NOT be changed without adding or
                        * removing elements from the vbyte structure!     */
#define NEIGHBORS_ON_GRID 8
#define FAILURE    -1  

#define DEAD  false
#define ALIVE true

typedef struct square {
	int y;
	int x;
} square;

/* 
 * Virtual byte to aid memory usage while sidestepping common machine
 * dependency issues. The choice not to simply store everthing as an array
 * of booleans was perhaps an error!
 */ 
static struct vbyte {
	bool b1 : 1;
	bool b2 : 1;
	bool b3 : 1;
	bool b4 : 1;
	bool b5 : 1;
	bool b6 : 1;
	bool b7 : 1;
	bool b8 : 1;
}

/*
 * Used to address a particular bit from field. Necessary as a medium between
 * the memory format of vbytes/vbits & the I/O rows/columns
 */
static struct vbit {
	int vbyteaddr;
	unsigned char vbitaddr;
}

static struct vbyte *field, *fieldbuffer;  /* The game board stored as a 
																						* group of vbytes            */          	
static int vbytec;           /* Number of vbytes in field */
static int rowc, columnc, spacec;
static bool initialized = false;     

/*
 * Initializes the game in memory. This must be called before any other
 * external functions belonging to this module. ALWAYS call lexit() to mirror
 * every call to linit to prevent a memory leak.
 */
int
linit(int rows, int columns) {

	assert(initialized == false);

	/* Validate inputs */
	assert(rows > 0);
	assert(columns > 0);
	assert((INT_MAX / columns) > rows);
	

	spacec = rowc * columnc;
  vbytec = spacec / VBYTE_SIZE;
	if (needsevb()) {
		vbytec++;
	}

	/* Make sure memory gets allocated. */
	if ((field = calloc((size_t) vbytec, sizeof(vbyte))) == NULL) {
		return FAILURE;
	}

	initialized = true;
	return 0;
}

int
tick() {
	static square activesquare = {0, 0};
	
	if ((fieldbuffer = calloc((size_t) vbytec, sizeof(vbyte))) == NULL){
		return FAILURE;
	}

	static int activeneighbors = 0;
	
	while(activesquare.y++ < linec) {
		activesquare.x = 0;

		while(activesquare.x++ < rowc) {
			activeneighbors = getactiveneighbors(activesquare);
			if (activeneighbors < 2 || activeneighbors > 3){
				setsquare(activesquare, DEAD);
			}
			else if (activeneighbors == 3){
				setsquare(activesquare, ALIVE);
			}
			else if ((getsquare(activesquare) == ALIVE) && activeneighbors == 2){
				setsquare(activesquare, ALIVE);
			}
		}
	}
	memcpy(field, fieldbuffer, ((size_t) vbytec * sizeof(vbyte));
	free(fieldbuffer);

}

int
lexit(){
	assert(initialized == true);
	free(field);
	return 0;
}

static int
getactiveneighbors(square givensquare){
	activeneighbors = 0;
	static square attemptsquare;
	
	for (int i = 0; i < NEIGHBORS_ON_GRID; i++) {
		attemptsquare.x = givensquare.x;
		attemptsquare.y = givensquare.y;
		switch i{
		case 0:
			attemptsquare.x--;
			attemptsquare.y--;
			break;
		case 1:
			attemptsquare.y--;
			break;
		case 2:
			attemptsquare.x++;
			attemptsquare.y--;
			break;
		case 3:
			attemptsquare.x++;
			break;
		case 4:
			attemptsquare.x++;
			attemptsquare.y++;
			break;
		case 5:
			attemptsquare.y++;
			break;
		case 6:
			attemptsquare.x--;
			attemptsquare.y++;
			break;
		case 7:
			attemptsquare.x--;
			break;
		}
		if (squareexists(attemptsquare)) {
			if (squareisactive(attemptsquare)){
				activeneighbors++;
			}
		}
	}

	return activeneighbors;
}

static bool
squareisactive(square givensquare){
	return getfieldbit(squaretovbit(givensquare));
}

static int
setsquare(square sentsquare, bool value){
	return setfieldbufferbit((squaretovbit(givensquare)), value);
}

static bool
getfieldbit(struct vbit vbitref) {

	/* Validate inputs */
	assert(vbitref.vbyteaddr >= 0 && vbitref.vbyteaddr <  vbytec);
 	assert(vbitref.vbitaddr  >= 0 && vbitref.vbitaddr  <	VBYTE_SIZE);

	switch (vbitaddr){
	
	case 0:	return field[vbitref.vbyteaddr].b1;
	case 1:	return field[vbitref.vbyteaddr].b2;
	case 2:	return field[vbitref.vbyteaddr].b3;
	case 3:	return field[vbitref.vbyteaddr].b4;
	case 4:	return field[vbitref.vbyteaddr].b5;
	case 5:	return field[vbitref.vbyteaddr].b6;
	case 6:	return field[vbitref.vbyteaddr].b7;
	case 7:	return field[vbitref.vbyteaddr].b8;
	
	default:
		exit(FAILURE);	
}

static int
setfieldbufferbit(struct vbit vbitref, bool value) {
	
	/* Validate inputs */
	assert(vbitref.vbyteaddr >= 0 && vbitref.vbyteaddr <  vbytec);
 	assert(vbitref.vbitaddr  >= 0 && vbitref.vbitaddr  <	VBYTE_SIZE);

	switch (vbitaddr){
	
	case 0:	fieldbuffer[vbitref.vbyteaddr].b1 = value;
	case 1:	fieldbuffer[vbitref.vbyteaddr].b2 = value;
	case 2:	fieldbuffer[vbitref.vbyteaddr].b3 = value;
	case 3:	fieldbuffer[vbitref.vbyteaddr].b4 = value;
	case 4:	fieldbuffer[vbitref.vbyteaddr].b5 = value;
	case 5:	fieldbuffer[vbitref.vbyteaddr].b6 = value;
	case 6:	fieldbuffer[vbitref.vbyteaddr].b7 = value;
	case 7:	fieldbuffer[vbitref.vbyteaddr].b8 = value;

	default:
		return FAILURE;	
	}
}
static struct vbit
squaretovbit(square sentsquare) {
	int space;
  struct vbit requestedvbit;

	/* Validate inputs */
	assert(sentsquare.y >= 0 && sentsquare.y < rowc);
	assert(sentsquare.x >= 0 && sentsquare.x < columnc);

	space = (sentsquare.y * columnc) + sentsquare.x;

	requestedvbit.vbyteaddr = space / VBYTE_SIZE;
	requestedvbit.vbitaddr  = space % VBYTE_SIZE;

	return requestedvbit;
}

static square
vbittosquare(struct vbit sentvbit) {
	int absaddr;
	square requestedsquare;

	/* Validate inputs */
	assert(sentvbit.vbyteaddr >= 0 && sentvbit.vbyteaddr <  vbytec);
 	assert(sentvbit.vbitaddr  >= 0 && sentvbit.vbitaddr  <	VBYTE_SIZE);

	absaddr = (sentvbit.vbyteaddr * VBYTE_SIZE) + vbitaddr;

	requestedsquare.y = absaddr / columnc;
	requestedsquare.x = absaddr % columnc;

	return requestedsquare;
}

/*
 * Tells whether an extra vbyte needs to be added given a certain amount
 * of spaces; floor division can return improper results.
 */
static bool
needsevb(int spaces) {
  ((spaces % VBYTE_SIZE) == 0)
	? return false
	: return true
}

struct bool
squareexists(square sentsquare){
	if (square.x < 0 || square.x >= columnc){
		return false;
	}
	if (square.y < 0 || square.y >= linec){
		return false;
	}
	return true;
}
