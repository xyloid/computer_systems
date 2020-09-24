// Used by time_stack.h
#define TIME_UNIT struct timespec

// Use a stack to track function enter and exit.
#include "time_stack.h"
#include <stdio.h>
#include <time.h>

static void *MAIN = NULL;
static unsigned int counter = 0;

static stack_t *record;

__attribute__((no_instrument_function)) void
__cyg_profile_func_enter(void *this_fn, void *call_site) {
	// The first function entered is main.
	if (MAIN == NULL) {
		MAIN = this_fn;
		record = create_stack(INT_MAX);
	}
	counter++;

	// Get and push start time.
	TIME_UNIT start;
	clock_gettime(CLOCK_REALTIME, &start);
	stack_enqueue(record, start);
	printf("\tFunction entered %p from %p\n", this_fn, call_site);
}

__attribute__((no_instrument_function)) void
__cyg_profile_func_exit(void *this_fn, void *call_site) {
	// Get end time.
	TIME_UNIT end;
	clock_gettime(CLOCK_REALTIME, &end);
	printf("\tFunction %p exited to %p\n", this_fn, call_site);

	// Pop from time stack, which return the start time of this function.
	TIME_UNIT start = stack_dequeue(record);
	// Compute high resolution time in micro second.
	double interval =
	    (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3;

	printf("\t    func %p time elpased : %.2f microseconds; %.2f seconds\n",
	       this_fn, interval, interval / 1e6);

	// If the function exited is main, then print total function call number.
	if (this_fn == MAIN) {
		printf("\nTotal Functions called = %d\n\n", counter);
		free_stack(record);
	}
}
