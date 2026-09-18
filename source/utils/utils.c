/*
 * utils.c - Miscellaneous helpers shared by the whole codebase.
 */
#include "utils.h"

#include <sys/time.h>

/* Scratch buffer for the clock reads below; both getters share it. */
STATIC struct timeval tv;


/* In-place ASCII upper-casing of the first `size` bytes of `str`. */
PUBLIC void str_to_upper(char* str, int size) {
    for (int i = 0; i < size; ++i) {
        char c = str[i];

        if (c >= 'a' && c <= 'z') {
            str[i] = c - ('a' - 'A');
        }
    }
}


/* Wall clock in microseconds, used to pace ticks and to measure fps. */
PUBLIC unsigned long long int get_time_us() {
    gettimeofday(&tv, 0);

    return (tv.tv_sec * 1000000) + (tv.tv_usec);
}


/* Wall clock in milliseconds. */
PUBLIC unsigned long long int get_time_ms() {
    return get_time_us() / 1000;
}


/* Absolute value of a double, for code that would rather avoid math.h. */
PUBLIC double dabs(double d) {
    return d < 0 ? -d : d;
}
