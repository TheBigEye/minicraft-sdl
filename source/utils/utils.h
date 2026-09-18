/*
 * utils.h - Small platform helpers: string casing, the wall clock and an
 *           absolute value for doubles.
 *
 * Java has no counterpart for these; they stand in for what the original
 * gets from the JDK, chiefly System.currentTimeMillis().
 */
#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_ 1

#include "javalang.h"

/* Upper-cases the first `size` bytes of `str` in place (ASCII letters only). */
PUBLIC void str_to_upper(char* str, int size);

/* Microseconds since the epoch; drives the main loop timing. */
PUBLIC unsigned long long int get_time_us();

/* The same clock, in milliseconds. */
PUBLIC unsigned long long int get_time_ms();

/* Absolute value of a double. */
PUBLIC double dabs(double d);

#endif /* UTILS_UTILS_H_ */
