/*
 * Internal logic for the game of life. linit() must be called to initialize
 * the board before anything else.
 */

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

#include "life.h"

#define VBYTE_SIZE         8   /* Size of a vbyte. Struct must aggree. */
#define NEIGHBORS_ON_GRID  8   /* Orthogonal/diagonal neighbors of square */


/* 
 * Virtual byte to aid memory usage while sidestepping common machine
 * dependency issues. The choice not to simply store everthing as an array
 * of booleans was perhaps an error!
 */ 
struct vbyte {
	bool b1 : 1;
	bool b2 : 1;
	bool b3 : 1;
	bool b4 : 1;
	bool b5 : 1;
	bool b6 : 1;
	bool b7 : 1;
	bool b8 : 1;
};

/*
 * Used to address a particular bit from field. Necessary as a medium between
 * the memory format of vbytes/vbits & the I/O rows/columns
 */
struct vbit {
	int vbyteaddr;
	int vbitaddr;
};

static struct vbyte *field, *fieldbuffer; /* vbyte game board */          	
static int vbytec;                        /* Number of vbytes in field */
static int rowc, columnc, spacec;
static bool initialized = false;     

int  linit(int, int);
int  ltick(void);
int  lexit(void);
bool squareisalive(square_t);
int  setsquare(square_t, bool);

static int         setbuffersquare(square_t, bool);
static int         getactiveneighbors(square_t);
static bool        getfieldbit(struct vbit);
static int         setfieldbit(struct vbit, bool);
static int         setfieldbufferbit(struct vbit, bool);
static struct vbit squaretovbit(square_t);

/* Currently unused: */
/* static square_t    vbittosquare(struct vbit); */ 

static bool        needsevb(int);
static bool        squareexists(square_t);

/*
 * Initializes the game in memory. This must be called before any other
 * external functions belonging to this module. ALWAYS call lexit() to mirror
 * every call to linit to prevent a memory leak.
 */
int
linit(int rows, int columns) {

	assert(initialized != true);

	/* Validate inputs */
	assert(rows > 0);
	assert(columns > 0);
	assert((INT_MAX / columns) > rows);
	
	rowc = rows;
	columnc = columns;
	spacec = rowc * columnc;
  vbytec = spacec / VBYTE_SIZE;
	if (needsevb(spacec)) {
		vbytec++;
	}

	/* Make sure memory gets allocated. */
	if ((field = (struct vbyte *) calloc((size_t) vbytec, sizeof(struct vbyte))) == NULL){
		return L_FAILURE;
	}

	initialized = true;
	return L_SUCCESS;
}

/*
 * Causes a tick to pass. Every square has its neighbors checked and adjusts
 * accordingly. Modifies field.
 */
int
ltick() {
	square_t activesquare = {-1, -1};
	
	if ((fieldbuffer = (struct vbyte *) calloc((size_t) vbytec, sizeof(struct vbyte))) == NULL){
		return L_FAILURE;
	}

	int activeneighbors = 0;
	
	while(++activesquare.y < rowc) {
		activesquare.x = 0;

		while(++activesquare.x < columnc) {
			activeneighbors = getactiveneighbors(activesquare);
			if (activeneighbors < 2 || activeneighbors > 3){
				if (setbuffersquare(activesquare, DEAD) == L_FAILURE){
					return L_FAILURE;	
				}
			}
			else if (activeneighbors == 3){
				if (setbuffersquare(activesquare, ALIVE) == L_FAILURE){
					return L_FAILURE;		
				}
			}
			else if ((squareisalive(activesquare) == ALIVE) && activeneighbors == 2){
				if (setbuffersquare(activesquare, ALIVE) == L_FAILURE){
					return L_FAILURE;
				}
			}
		}
	}
	memcpy(field, fieldbuffer, ((size_t) vbytec * sizeof(struct vbyte)));
	free(fieldbuffer);
	return L_SUCCESS;
}


/*
 * Safely exits the game of life. Frees memory allocated to field.
 * Returns L_SUCCESS if lexit terminates normally. Returns L_FAILURE otherwise.
 */
int
lexit(){
	assert(initialized == true);
	free(field);
	return L_SUCCESS;
}

/*
 * True if square is alive, false otherwise.
 */
bool
squareisalive(square_t givensquare){
	return getfieldbit(squaretovbit(givensquare));
}

int
setsquare(square_t sentsquare, bool value){
		return setfieldbit((squaretovbit(sentsquare)), value); 
}

/*
 * Returns L_SUCCESS when executed successfully; L_FAILURE otherwise.
 */
int
setbuffersquare(square_t sentsquare, bool value){
	return setfieldbufferbit((squaretovbit(sentsquare)), value);
}

/*
 * Returns the amount of living neighbors for a given square. Returns L_SUCCESS
 * when it terminates successfully; L_FAILURE otherwise.
 */
int
getactiveneighbors(square_t givensquare){
	int activeneighbors = 0;
	square_t attemptsquare;
	
	for (int i = 0; i < NEIGHBORS_ON_GRID; i++) {
		attemptsquare.x = givensquare.x;
		attemptsquare.y = givensquare.y;
		switch (i){
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
			if (squareisalive(attemptsquare)){
				activeneighbors++;
			}
		}
	}

	return activeneighbors;
}


/*
 * Returns true if a given bit that belongs to field is set to true; false if
 * otherwise.
 */
bool
getfieldbit(struct vbit vbitref) {

	/* Validate inputs */
	assert(vbitref.vbyteaddr >= 0 && vbitref.vbyteaddr <  vbytec);
 	assert(vbitref.vbitaddr  >= 0 && vbitref.vbitaddr  <	VBYTE_SIZE);

	switch (vbitref.vbitaddr){
	
	case 0:	return field[vbitref.vbyteaddr].b1;
	case 1:	return field[vbitref.vbyteaddr].b2;
	case 2:	return field[vbitref.vbyteaddr].b3;
	case 3:	return field[vbitref.vbyteaddr].b4;
	case 4:	return field[vbitref.vbyteaddr].b5;
	case 5:	return field[vbitref.vbyteaddr].b6;
	case 6:	return field[vbitref.vbyteaddr].b7;
	case 7:	return field[vbitref.vbyteaddr].b8;
	
	default:
		fprintf(stderr, "Error: vbit address out of range!");
		exit(EXIT_FAILURE);	
	}
}

/*
 * Sets a given vbit to the argument value. Returns L_SUCCESS when executed
 * successfully; L_FAILURE otherwise.
 */
int
setfieldbit(struct vbit vbitref, bool value) {
	
	/* Validate inputs */
	assert(vbitref.vbyteaddr >= 0 && vbitref.vbyteaddr <  vbytec);
 	assert(vbitref.vbitaddr  >= 0 && vbitref.vbitaddr  <	VBYTE_SIZE);

	switch (vbitref.vbitaddr){
	
	case 0:	field[vbitref.vbyteaddr].b1 = value; break;
	case 1:	field[vbitref.vbyteaddr].b2 = value; break;
	case 2:	field[vbitref.vbyteaddr].b3 = value; break;
	case 3:	field[vbitref.vbyteaddr].b4 = value; break;
	case 4:	field[vbitref.vbyteaddr].b5 = value; break;
	case 5:	field[vbitref.vbyteaddr].b6 = value; break;
	case 6:	field[vbitref.vbyteaddr].b7 = value; break;
	case 7:	field[vbitref.vbyteaddr].b8 = value; break;

	default:
		return L_FAILURE;	
	}
	return L_SUCCESS;
}

/*
 * Sets the bit given by argument vbitref to argument value. Returns L_SUCCESS
 * when successfully set; L_FAILURE otherwise.
 */
int
setfieldbufferbit(struct vbit vbitref, bool value) {
	
	/* Validate inputs */
	assert(vbitref.vbyteaddr >= 0 && vbitref.vbyteaddr <  vbytec);
 	assert(vbitref.vbitaddr  >= 0 && vbitref.vbitaddr  <	VBYTE_SIZE);

	switch (vbitref.vbitaddr){
	
	case 0:	fieldbuffer[vbitref.vbyteaddr].b1 = value; break;
	case 1:	fieldbuffer[vbitref.vbyteaddr].b2 = value; break;
	case 2:	fieldbuffer[vbitref.vbyteaddr].b3 = value; break;
	case 3:	fieldbuffer[vbitref.vbyteaddr].b4 = value; break;
	case 4:	fieldbuffer[vbitref.vbyteaddr].b5 = value; break;
	case 5:	fieldbuffer[vbitref.vbyteaddr].b6 = value; break;
	case 6:	fieldbuffer[vbitref.vbyteaddr].b7 = value; break;
	case 7:	fieldbuffer[vbitref.vbyteaddr].b8 = value; break;

	default:
		return L_FAILURE;	
	}
	return L_SUCCESS;
}

/*
 * Converts argument sentsquare to vbit format and returns the vbit.
 */
struct vbit
squaretovbit(square_t sentsquare) {
	int space;
  struct vbit requestedvbit;

	/* Validate inputs */
	assert(sentsquare.y >= 0);
	assert(sentsquare.y < rowc);
	assert(sentsquare.x >= 0);
	assert(sentsquare.x < columnc);

	space = (sentsquare.y * columnc) + sentsquare.x;

	requestedvbit.vbyteaddr = space / VBYTE_SIZE;
	requestedvbit.vbitaddr  = space % VBYTE_SIZE;

	return requestedvbit;
}

/*
 * Converts argument sentvbit to square format and returns the square.
 * Currently unused!
 */

/*
square_t
vbittosquare(struct vbit sentvbit) {
	int absaddr;
	square_t requestedsquare;

	* Validate inputs *
	assert(sentvbit.vbyteaddr >= 0 && sentvbit.vbyteaddr <  vbytec);
 	assert(sentvbit.vbitaddr  >= 0 && sentvbit.vbitaddr  <	VBYTE_SIZE);

	absaddr = (sentvbit.vbyteaddr * VBYTE_SIZE) + sentvbit.vbitaddr;

	requestedsquare.y = absaddr / columnc;
	requestedsquare.x = absaddr % columnc;

	return requestedsquare;
}

 */

/*
 * Tells whether an extra vbyte needs to be added given a certain amount
 * of spaces; floor division can return improper results.
 */
bool
needsevb(int spaces) {
	bool doesneed;
  doesneed = ((spaces % VBYTE_SIZE) == 0)
	? false
	: true;
	return doesneed;
}

/*
 * Returns true if a given square exists in memory; false otherwise.
 */
bool
squareexists(square_t sentsquare){
	if (sentsquare.x < 0 || sentsquare.x >= columnc){
		return false;
	}
	if (sentsquare.y < 0 || sentsquare.y >= rowc){
		return false;
	}
	return true;
}
