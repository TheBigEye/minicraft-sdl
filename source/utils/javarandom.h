/*
 * javarandom.h - Deterministic PRNG identical to java.util.Random.
 *
 * Level generation must reproduce the same world as the original game
 * for a given seed, so this mirrors Java's 48-bit linear congruential
 * generator exactly, including the cached second Gaussian sample.
 */
#ifndef UTILS_JAVARANDOM_H_
#define UTILS_JAVARANDOM_H_ 1

typedef struct{
	long long int seed;         /* 48-bit LCG state */
	float nextNextGaussian;     /* cached second sample of a Box-Muller pair */
	char haveNextNextGaussian;  /* whether the cached Gaussian is valid */
} Random;

/* Seeds the generator the same way java.util.Random.setSeed() does. */
void random_set_seed(Random* random, int seed);
/* Advances the 48-bit state and returns its `bits` highest bits. */
int random_next(Random* random, int bits);
/* Uniform int in [0, bound), same rejection logic as Java's nextInt. */
int random_next_int(Random* random, int bound);
/* Uniform float in [0, 1). */
float random_next_float(Random* random);
/* Normally distributed sample (mean 0, std dev 1), Java's nextGaussian. */
float random_next_gaussian(Random* random);
/* Random boolean, i.e. one random bit. */
char random_next_boolean(Random* random);

#endif /* UTILS_JAVARANDOM_H_ */
