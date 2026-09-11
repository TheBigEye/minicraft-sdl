/*
 * utils.c - Miscellaneous helpers shared by the whole codebase.
 */
#include "utils.h"
#include <sys/time.h>

/* Shared scratch buffer for the clock reads below. */
struct timeval tv;


/* In-place ASCII upper-casing of the first `size` bytes of `str`. */
void strToUpper(char* str, int size) {
	for (int i = 0; i < size; ++i) {
		char c = str[i];
		if (c >= 'a' && c <= 'z') str[i] = c - ('a' - 'A');
	}
}


/* Wall clock in microseconds, used to pace ticks and measure fps. */
unsigned long long int getTimeUS() {
	gettimeofday(&tv, 0);
	return (tv.tv_sec * 1000000) + (tv.tv_usec);
}


/* Wall clock in milliseconds. */
unsigned long long int getTimeMS() {
	return getTimeUS() / 1000;
}


/* Absolute value of a double, for code that avoids math.h. */
double dabs(double d) {
	return d < 0 ? -d : d;
}
