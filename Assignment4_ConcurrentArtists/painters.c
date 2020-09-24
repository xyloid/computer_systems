// Compile by either typing 'make' in the terminal
// or manually with 'clang -lpthread painters.c -o painters'

// ===================== Include Libraries =====================

// For compiling random_r in gilbc <=2.19 (Khoury server)
#define _SVID_SOURCE
// For compiling random_r in glibc >=2.19 (my ubuntu)
#define _DEFAULT_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
// ===================== Setup our Canvas =====================
#define CANVAS_WIDTH 256
#define CANVAS_HEIGHT 256

// 8-bit color
#define COLORS_NUM 256

// We want more complex random number in thread-safe random_r.
#define RN_BF_SZ 64

// Our canvas is a shared piece of memory that each artist can access.
// Within our canvas, every pixel has a red, green, and blue component.
// Because we only want one artist at a time drawing on a pixel
// each pixel also has a lock associated with it.
typedef struct pixel {
	int r, g, b;
	pthread_mutex_t lock;
} pixel_t;

// Create our canvas as a global variable.
// This canvas will be shared amongst all of our threads.
pixel_t canvas[CANVAS_WIDTH][CANVAS_HEIGHT];

// ===================== Setup our Artists =====================
// This is our data structure for each artist
typedef struct artist {
	int x, y;    // Store the position of where an artist is painting
	int r, g, b; // The color an artist paints in.
} artist_t;

// An artist can move in one of the following directions
// This is a handy abstraction, as I can 'roll an 8-sided dice'
// and depending on what value that is, move in an x and y position
// corresponding with the index below.
// e.g.
// A roll of 0 moves my artist (-1,0).
// A roll of 7 moves my artist (-1,-1)
const int moves = 8;
const int movement[][2] = {
    {-1, 0}, // to the left
    {-1, 1}, // to the left and down
    {0, 1},  // down
    {1, 1},  // to the right and down
    {1, 0},  // to the right
    {1, -1}, // to the right and up
    {0, -1}, // up
    {-1, -1} // to the left and up
};

// This function sets up the canvas
// The purpose is to iterate through every pixel and
// setup the initial values of the pixel and initialize the
// lock structure.
// The intent is to call this function early in main.
void initCanvas() {
	for (int x = 0; x < CANVAS_WIDTH; ++x) {
		for (int y = 0; y < CANVAS_HEIGHT; ++y) {
			canvas[x][y].r = 255;
			canvas[x][y].g = 255;
			canvas[x][y].b = 255;
			pthread_mutex_init(&canvas[x][y].lock, NULL);
		}
	}
}

// This function saves the canvas as a PPM.
// This function should be called after all painting
// operations have completed.
void outputCanvas() {
	// Write out the PPM file
	// If you are looking at this solution, it could
	// be better laid out in a 'save function'
	FILE *fp;
	fp = fopen("canvas.ppm", "w+");
	fputs("P3\n", fp);
	fputs("256 256\n", fp);
	fputs("255\n", fp);
	for (int x = 0; x < CANVAS_WIDTH; ++x) {
		for (int y = 0; y < CANVAS_HEIGHT; ++y) {
			fprintf(fp, "%d", canvas[x][y].r);
			fputs(" ", fp);
			fprintf(fp, "%d", canvas[x][y].g);
			fputs(" ", fp);
			fprintf(fp, "%d", canvas[x][y].b);
			fputs(" ", fp);
		}
		fputs("\n", fp);
	}
	fclose(fp);
}

// I want all child threads start at almost same time.
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int artists_number = 4;

int compare_pixel(int xa, int ya, int xb, int yb) {
	if (canvas[xa][ya].r == canvas[xb][yb].r &&
	    canvas[xa][ya].g == canvas[xb][yb].g &&
	    canvas[xa][ya].b == canvas[xb][yb].b) {
		return 1;
	} else {
		return 0;
	}
}

// TODO: You will make code changes here.
// Here is our thread function for painting with no locking implemented.
// You may modify this code however you like.
void *paint(void *args) {
	struct random_data *rand_states;
	char *rand_statebufs;
	rand_states = (struct random_data *)calloc(1, sizeof(struct random_data));
	rand_statebufs = (char *)calloc(1, RN_BF_SZ);
	initstate_r(random(), rand_statebufs, RN_BF_SZ, rand_states);
	// Convert our argument structure to an artist
	artist_t *painter = (artist_t *)args;

	// Record possible moves.
	int mapping[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	// Number of possible moves.
	int possible_moves = moves;
	int counter = 0;

	// Trying to make them start at about same time.
	pthread_mutex_lock(&mutex);
	while (artists_number < 54) {
		pthread_cond_wait(&cond, &mutex);
	}
	pthread_mutex_unlock(&mutex);

	// Our artist will now attempt to paint 5000 strokes of paint
	// on our shared canvas
	for (int i = 0; i < 5000; ++i) {

		// Store our initial position
		int currentX = painter->x;
		int currentY = painter->y;
		// Generate a random number from 0-7
		int roll;
		random_r(rand_states, &roll);

		roll = roll % possible_moves;
		painter->x += movement[mapping[roll]][0];
		painter->y += movement[mapping[roll]][1];

		// Clamp the range of our movements so we only
		// paint within our 256x256 canvas.
		if (painter->x < 0) {
			painter->x = 0;
		}
		if (painter->x > CANVAS_WIDTH - 1) {
			painter->x = CANVAS_WIDTH - 1;
		}
		if (painter->y < 0) {
			painter->y = 0;
		}
		if (painter->y > CANVAS_HEIGHT - 1) {
			painter->y = CANVAS_HEIGHT - 1;
		}

		// TODO: Implement some locking mechanism
		// at first glance this seems okay, but convince yourself
		// we can still have data races.
		// I suggest investigating a 'trylock'
		if (pthread_mutex_trylock(&canvas[painter->x][painter->y].lock) == 0) {

			// Try to paint
			// paint the pixel if it is white or its same color.
			if ((canvas[painter->x][painter->y].r == 255 &&
			     canvas[painter->x][painter->y].g == 255 &&
			     canvas[painter->x][painter->y].b == 255) ||
			    (canvas[painter->x][painter->y].r == painter->r &&
			     canvas[painter->x][painter->y].g == painter->g &&
			     canvas[painter->x][painter->y].b == painter->b)) {

					 
				if (canvas[painter->x][painter->y].r == 255 &&
				    canvas[painter->x][painter->y].g == 255 &&
				    canvas[painter->x][painter->y].b == 255) {

					// Compute variables to check if there is a valid move.
					// In order to eliminate pontential overlap caused by
					// diagonal moves.
					/*
					 *	   XOOO
					 *     OXOO   <--- I assume this is overlap.
					 *     OOXX
					 * */

					// A solution to the above problem:
					/*
					 * Before move form O to o, check if at least one of the
					 * _'s is O. Thus, the artist won't trespass two diagonal
					 * pixels belonging to other artist.
					 *
					 *  	O_
					 *  	_o
					 *
					 * */
					int dx = painter->x - currentX;
					int dy = painter->y - currentY;
					int square = dx * dx + dy * dy;
					// The following two operation will access two adjacent
					// pixels. Although they look like there could be a race
					// condition, in this assignment settings, there won't be
					// any undeterministic result. This is because the
					// compare_pixel function will return 1 only when the
					// adjacent pixel has the same color as the current pixel.
					// And if this is true, then the adjacent pixel color will
					// never be changed even there are some other artists trying
					// to change it. If compare_pixel returns 0, then the
					// adjacent pixel is either white or being modified by other
					// artist. Even if compare_pixel get an mixed inconsistent
					// pixel reading of those two states, it will still return
					// the correct value 0.

					int cmp1 =
					    compare_pixel(currentX, painter->y, currentX, currentY);
					int cmp2 =
					    compare_pixel(painter->x, currentY, currentX, currentY);
					int cmp_result = cmp1 + cmp2;

					if (square == 1 || (square == 2 && cmp_result > 0)) {
						counter++;
						canvas[painter->x][painter->y].r = painter->r;
						canvas[painter->x][painter->y].g = painter->g;
						canvas[painter->x][painter->y].b = painter->b;
					} else {
						// Although locked the pixel, but the artist can not 
						// paint it.
						pthread_mutex_unlock(
						    &canvas[painter->x][painter->y].lock);
						painter->x = currentX;
						painter->y = currentY;
						possible_moves--;
						// Move invalid direction to the valid last position of
						// mapping. Next direction is chosen from the valid set.
						int temp = mapping[possible_moves];
						mapping[possible_moves] = mapping[roll];
						mapping[roll] = temp;
						if (possible_moves <= 0) {
							// Pass -DINFO to gcc to enable print running
							// infomation. Check Makefile. It's has been
							// provided in some targets. 'make fixd' or 'make
							// randd' An interesting observation is that only
							// the artist whose start pixel is surrounded by 8
							// pixels that already be painted by other artists
							// will be completely stuck. This is because artist
							// has more than one pixels painted will moving
							// around his area until his paint runs out.
#ifdef INFO
							printf("Painter %d %d %d at %d %d painted %d STUCK "
							       "BC NO TRESPASS\n",
							       painter->r, painter->g, painter->b,
							       painter->x, painter->y, counter);
#endif

							break;
						} else {
							// To next loop.
							continue;
						}
					}
				}
				pthread_mutex_unlock(&canvas[painter->x][painter->y].lock);
				// Reset mapping.
				int j;
				for (j = 0; j < moves; j++) {
					mapping[j] = j;
				}
				possible_moves = moves;
			} else {
				// If we cannot paint the pixel, then we backtrack
				// to a previous pixel that we own.
				painter->x = currentX;
				painter->y = currentY;
				possible_moves--;
				// Move invalid direction to the valid last position of mapping.
				// Next direction is chosen from the valid set.
				int temp = mapping[possible_moves];
				mapping[possible_moves] = mapping[roll];
				mapping[roll] = temp;
				if (possible_moves <= 0) {
					// Pass -DINFO to gcc to enable print running infomation.
					// Check Makefile. It's has been provided in some targets.
					// 'make fixd' or 'make randd'
					// An interesting observation is that only the artist whose
					// start pixel is surrounded by 8 pixels that already be
					// painted by other artists will be completely stuck. This
					// is because artist has more than one pixels painted will
					// moving around his area until his paint runs out.
#ifdef INFO
					printf("Painter %d %d %d at %d %d painted %d STUCK\n",
					       painter->r, painter->g, painter->b, painter->x,
					       painter->y, counter);
#endif
					break;
				}
			}

		} else {
			// If we cannot paint the pixel, then we backtrack
			// to a previous pixel that we own.
			// In this situation, we do nothing, as we don't know what happens
			// to the aim pixel.
			painter->x = currentX;
			painter->y = currentY;
		}
	}// For loop
	free(rand_states);
	free(rand_statebufs);
#ifdef INFO
	printf("Painter %d %d %d at %d %d painted %d \n", painter->r, painter->g,
	       painter->b, painter->x, painter->y, counter);
#endif
	return NULL;
}

// Record which color is assgined.
static char palette[COLORS_NUM][COLORS_NUM][COLORS_NUM] = {{{0}}};
// Record which start pixel is assigned.
static char origins[CANVAS_WIDTH][CANVAS_HEIGHT] = {{0}};

// Set a square whose center is (x,y) as 1 in origins[][].
// So the artists' start pixels won't be too close to each other.
void canvas_divide(int x, int y, int distance) {
	int i;
	int j;
	for (i = x - distance; i <= x + distance; i++) {
		if (x >= 0 && x < CANVAS_WIDTH) {
			continue;
		} else {
			for (j = y - distance; j <= y + distance; j++) {
				if (j >= 0 && j < CANVAS_HEIGHT) {
					origins[i][j] = 1;
				}
			}
		}
	}
}

// ================== Program Entry Point ============
int main() {

	pthread_mutex_lock(&mutex);

	// Random number generate for main process.
	struct random_data *rand_states;
	char *rand_statebufs;
	rand_states = (struct random_data *)calloc(1, sizeof(struct random_data));
	rand_statebufs = (char *)calloc(1, RN_BF_SZ);
	initstate_r(random(), rand_statebufs, RN_BF_SZ, rand_states);

	// Initialize our 'blank' canvas
	initCanvas();

	// Our four expert artists
	artist_t *Michaelangelo = malloc(sizeof(artist_t));
	artist_t *Donatello = malloc(sizeof(artist_t));
	artist_t *Raphael = malloc(sizeof(artist_t));
	artist_t *Leonardo = malloc(sizeof(artist_t));

	// Fill in the artist attributes
	// You will see below how this structure is used
	// as arguments passed into our thread. This is a
	// simple and organized way to pass information along.
	Michaelangelo->x = 0;
	Michaelangelo->y = 0;
	Michaelangelo->r = 255;
	Michaelangelo->g = 0;
	Michaelangelo->b = 165;
	// Fill in the artist attributes
	Donatello->x = CANVAS_WIDTH - 1;
	Donatello->y = 0;
	Donatello->r = 128;
	Donatello->g = 0;
	Donatello->b = 128;
	// Fill in the artist attributes
	Raphael->x = CANVAS_WIDTH - 1;
	Raphael->y = CANVAS_HEIGHT - 1;
	Raphael->r = 255;
	Raphael->g = 0;
	Raphael->b = 0;
	// Fill in the artist attributes
	Leonardo->x = 0;
	Leonardo->y = CANVAS_HEIGHT - 1;
	Leonardo->r = 0;
	Leonardo->g = 0;
	Leonardo->b = 255;

	// Set first touch.
	// This step is to make sure each artist will paint at least one pixel in
	// the canvas.
	canvas[Michaelangelo->x][Michaelangelo->y].r = Michaelangelo->r;
	canvas[Michaelangelo->x][Michaelangelo->y].g = Michaelangelo->g;
	canvas[Michaelangelo->x][Michaelangelo->y].b = Michaelangelo->b;

	canvas[Donatello->x][Donatello->y].r = Donatello->r;
	canvas[Donatello->x][Donatello->y].g = Donatello->g;
	canvas[Donatello->x][Donatello->y].b = Donatello->b;

	canvas[Raphael->x][Raphael->y].r = Raphael->r;
	canvas[Raphael->x][Raphael->y].g = Raphael->g;
	canvas[Raphael->x][Raphael->y].b = Raphael->b;

	canvas[Leonardo->x][Leonardo->y].r = Leonardo->r;
	canvas[Leonardo->x][Leonardo->y].g = Leonardo->g;
	canvas[Leonardo->x][Leonardo->y].b = Leonardo->b;

	// Hold our thread id's
	pthread_t Michaelangelo_tid;
	pthread_t Donatello_tid;
	pthread_t Raphael_tid;
	pthread_t Leonardo_tid;
	// Initialize a seed for our random number generator
	srand(time(NULL));

	// Create our threads for each of our expert artists
	pthread_create(&Michaelangelo_tid, NULL, (void *)paint, Michaelangelo);
	pthread_create(&Donatello_tid, NULL, (void *)paint, Donatello);
	pthread_create(&Raphael_tid, NULL, (void *)paint, Raphael);
	pthread_create(&Leonardo_tid, NULL, (void *)paint, Leonardo);

	// Create a block of memory recording which color has been used.
	// It is initialized to 0.
	palette[Michaelangelo->r][Michaelangelo->g][Michaelangelo->b] = 1;
	palette[Donatello->r][Donatello->g][Donatello->b] = 1;
	palette[Raphael->r][Raphael->g][Raphael->b] = 1;
	palette[Leonardo->r][Leonardo->g][Leonardo->b] = 1;
	// Can not use background color.
	palette[255][255][255] = 1;
	// Create a block of memory recording which pixel has been used as a start
	// point.
	origins[Michaelangelo->x][Michaelangelo->y] = 1;
	origins[Donatello->x][Donatello->y] = 1;
	origins[Raphael->x][Raphael->y] = 1;
	origins[Leonardo->x][Leonardo->y] = 1;

	// TODO: Add 50 more artists
	int rookieArtists = 50;
	pthread_t moreArtists_tid[rookieArtists];
	artist_t *moreArtists =
	    (artist_t *)malloc(sizeof(artist_t) * rookieArtists);

	for (int i = 0; i < rookieArtists; ++i) {
		// Generate a color.
		// Pass -DFIX to gcc to fixed color assignment.
		// The default case is randomized color assignment.
		// Check Makefile. It's has been provided in some targets.
		// 'make' or 'make randd'
#ifndef FIX
		do {
			// Random color may generate indistinct colors.
			// But random color is beautiful.
			int r;
			random_r(rand_states, &r);
			moreArtists[i].r = (r % COLORS_NUM);
			random_r(rand_states, &r);
			moreArtists[i].g = (r % COLORS_NUM);
			random_r(rand_states, &r);
			moreArtists[i].b = (r % COLORS_NUM);
			// Do the while loop until a color is not assigned.
		} while (palette[moreArtists[i].r][moreArtists[i].g][moreArtists[i].b]);
		palette[moreArtists[i].r][moreArtists[i].g][moreArtists[i].b] = 1;
#else
		// Fixed color scheme, protecting testers' eyes.
		int coef_1 = i / 7;
		int coef_2 = i % 7;
		int r;
		random_r(rand_states, &r);
		moreArtists[i].r = coef_1 * 36;
		random_r(rand_states, &r);
		moreArtists[i].g = (7 - coef_2) * 36;
		random_r(rand_states, &r);
		moreArtists[i].b = (7 - coef_1) * 36;
		palette[moreArtists[i].r][moreArtists[i].g][moreArtists[i].b] = 1;
#endif
		// Generate a start position.
		do {
			int r;
			random_r(rand_states, &r);
			moreArtists[i].x = (r % CANVAS_WIDTH);
			random_r(rand_states, &r);
			moreArtists[i].y = (r % CANVAS_HEIGHT);
			// Do the while loop until a pixel is not assigned.
		} while (origins[moreArtists[i].x][moreArtists[i].y]);

		origins[moreArtists[i].x][moreArtists[i].y] = 1;
		// Add some distance between artists by marking the surrounding pixels
		// as assigned.
		canvas_divide(moreArtists[i].x, moreArtists[i].y, 15);

		// This step is to make sure each artist will paint at least one pixel
		// in the canvas.
		canvas[moreArtists[i].x][moreArtists[i].y].r = moreArtists[i].r;
		canvas[moreArtists[i].x][moreArtists[i].y].g = moreArtists[i].g;
		canvas[moreArtists[i].x][moreArtists[i].y].b = moreArtists[i].b;

		// Create threads.
		pthread_create(&moreArtists_tid[i], NULL, (void *)paint,
		               moreArtists + i);
	}

	// Release the artists.
	artists_number = 54;
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);

	// Join each with the main thread.
	// Do you think our ordering of launching each thread matters?
	pthread_join(Michaelangelo_tid, NULL);
	pthread_join(Donatello_tid, NULL);
	pthread_join(Raphael_tid, NULL);
	pthread_join(Leonardo_tid, NULL);

	// TODO: Add the join the 50 other artists threads here
	for (int i = 0; i < rookieArtists; ++i) {
		pthread_join(moreArtists_tid[i], NULL);
	}

	// Save our canvas at the end of the painting session
	outputCanvas();

	// Terminate our program
	free(Michaelangelo);
	free(Donatello);
	free(Raphael);
	free(Leonardo);

	// TODO: Free any other memory you can think of
	free(moreArtists);
	free(rand_states);
	free(rand_statebufs);

	return 0;
}
