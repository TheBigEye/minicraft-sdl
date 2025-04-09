#include "utils.h"
#include <sys/time.h>

struct timeval tv;


void strToUpper(char* str, int size) {
	for (int i = 0; i < size; ++i) {
		char c = str[i];
		if (c >= 'a' && c <= 'z') str[i] = c - ('a' - 'A');
	}
}


unsigned long long int getTimeUS() {
	gettimeofday(&tv, 0);
	return (tv.tv_sec * 1000000) + (tv.tv_usec);
}


unsigned long long int getTimeMS() {
	return getTimeUS() / 1000;
}


double dabs(double d) {
	return d < 0 ? -d : d;
}
