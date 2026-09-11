/*
 * javarandom.c - Reimplementation of java.util.Random's LCG.
 *
 * Same constants and same bit tricks as the Java class, so a seed always
 * yields the same sequence (and therefore the same generated levels) as
 * the original game.
 */
#include "javarandom.h"
#include <math.h>

/* java.util.Random's LCG constants: state = (state * 0x5DEECE66D + 0xB) mod 2^48 */
static const long long int multiplier = 0x5DEECE66D;
static const long long int mask = ((long long int)1 << 48) - 1;
static const long long int addend = 0xB;


/* Java's setSeed(): mix the seed with the multiplier and keep 48 bits. */
extern inline void random_set_seed(Random* random, int seed) {
	random->seed = (seed ^ multiplier) & mask;
	random->haveNextNextGaussian = 0;
}


/* Core of the generator: one LCG step, returning the `bits` top bits. */
extern inline int random_next(Random* random, int bits) {
	long long int oldseed = 0, nextseed = 0;
	long long int seed = random->seed;

	oldseed = seed;
	nextseed = (oldseed * multiplier + addend) & mask;

	random->seed = nextseed;

	return (int)((unsigned long long int) nextseed >> (48 - bits));
}


/* One random bit, as Java's nextBoolean(). */
extern inline char random_next_boolean(Random* random) {
	return random_next(random, 1) != 0;
}


/*
 * Uniform int in [0, bound). Powers of two take the fast multiply-shift
 * path; other bounds loop with rejection, exactly like Java's nextInt,
 * to keep the distribution unbiased (and the sequence identical).
 */
extern inline int random_next_int(Random* random, int bound) {
	int r = random_next(random, 31);
	int m = bound - 1;

	if (bound & m) {
		for (int u = r; u - (r = u % bound) + m < 0; u = random_next(random, 31));
	} else {
		r = ((bound * ((long long int) r)) >> 31);
	}

	return r;
}


/* Uniform float in [0, 1): 24 random bits scaled by 2^-24. */
extern inline float random_next_float(Random* random) {
	return random_next(random, 24) / (float)(1 << 24);
}


/* Uniform double in [0, 1): 53 random bits scaled by 2^-53. */
extern inline double random_next_double(Random* random) {
	return (((long long int) (random_next(random, 26)) << 27) + random_next(random, 27)) * (1.0 / (((long long int) 1) << 53));
}


/*
 * Java's nextGaussian(): polar (Box-Muller) method generating samples in
 * pairs. The second sample of the pair is cached in the Random struct and
 * returned on the next call, matching Java's behavior step by step.
 */
float random_next_gaussian(Random* random) {
	if (random->haveNextNextGaussian) {
		random->haveNextNextGaussian = 0;
		return random->nextNextGaussian;
	}

	float v1, v2, s;

	do {
		v1 = 2 * random_next_double(random) - 1;
		v2 = 2 * random_next_double(random) - 1;
		s = (v1 * v1) + (v2 * v2);
	} while (s >= 1 || s == 0);

	float multiplier = sqrt(-2 * log(s) / s);

	random->nextNextGaussian = v2 * multiplier;
	random->haveNextNextGaussian = 1;

	return v1 * multiplier;
}
