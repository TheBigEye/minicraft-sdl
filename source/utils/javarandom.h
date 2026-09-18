/*
 * javarandom.h - A deterministic generator identical to java.util.Random.
 *
 * Level generation has to reproduce the same world the original produces
 * for a given seed, so this replicates Java's 48-bit LCG exactly, right
 * down to the cached second gaussian sample.
 *
 * It is a class with methods, just like Java's Random: `r->next_int(r, n)`
 * is the counterpart of `r.nextInt(n)`.
 */
#ifndef UTILS_JAVARANDOM_H_
#define UTILS_JAVARANDOM_H_ 1

#include "javalang.h"

typedef struct Random Random;

/* Signatures of the Random methods. */
typedef void    (*random_set_seed_fn)     (Random* this, int seed);
typedef int     (*random_next_fn)         (Random* this, int bits);
typedef int     (*random_next_int_fn)     (Random* this, int bound);
typedef float   (*random_next_float_fn)   (Random* this);
typedef double  (*random_next_double_fn)  (Random* this);
typedef float   (*random_next_gaussian_fn)(Random* this);
typedef boolean (*random_next_boolean_fn) (Random* this);

struct Random {
    /* --- methods, installed by random_create() --- */

    /* Same as java.util.Random.setSeed(). */
    random_set_seed_fn set_seed;
    /* One step of the LCG, returning the highest `bits` bits. */
    random_next_fn next;
    /* Uniform int in [0, bound), with the same rejection as Java. */
    random_next_int_fn next_int;
    /* Uniform float in [0, 1). */
    random_next_float_fn next_float;
    /* Uniform double in [0, 1). */
    random_next_double_fn next_double;
    /* Normal, mean 0 and deviation 1: Java's nextGaussian(). */
    random_next_gaussian_fn next_gaussian;
    /* One random bit, that is, nextBoolean(). */
    random_next_boolean_fn next_boolean;

    /* --- data --- */

    /* State of the 48-bit LCG. */
    long long int seed;
    /* The second sample of the Box-Muller pair. */
    float nextNextGaussian;
    /* Whether the cached sample is valid. */
    boolean haveNextNextGaussian;
};

/*
 * Constructor: installs the methods. It has to run before the generator is
 * used; the Random objects embedded in other classes do it from their own
 * constructor, exactly where Java would write `new Random()`.
 */
PUBLIC void random_create(Random* this);

/* The method implementations. */
PUBLIC void    random_set_seed(Random* this, int seed);
PUBLIC int     random_next(Random* this, int bits);
PUBLIC int     random_next_int(Random* this, int bound);
PUBLIC float   random_next_float(Random* this);
PUBLIC double  random_next_double(Random* this);
PUBLIC float   random_next_gaussian(Random* this);
PUBLIC boolean random_next_boolean(Random* this);

#endif /* UTILS_JAVARANDOM_H_ */
