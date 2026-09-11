/*
 * utils.h - Small platform helpers: string casing, wall clock and a
 *           double absolute value.
 */
#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_ 1

/* Upper-cases `size` bytes of `str` in place (ASCII letters only). */
void strToUpper(char* str, int size);

/* Microseconds since the epoch; drives the main loop timing. */
unsigned long long int getTimeUS();
/* Same clock in milliseconds. */
unsigned long long int getTimeMS();

/* Absolute value of a double. */
double dabs(double d);

#endif /* UTILS_UTILS_H_ */
