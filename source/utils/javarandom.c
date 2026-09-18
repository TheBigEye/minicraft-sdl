/*
 * javarandom.c - Reimplementation of java.util.Random's LCG.
 *
 * Same constants and same bit tricks as the Java class, so a seed always
 * produces the same sequence, and therefore the same generated levels, as
 * the original game.
 *
 * Nothing here is `inline`: almost every function is reached through a
 * method pointer, and an `inline` function is not guaranteed to emit an
 * external definition, which would break the link.
 */
#include "javarandom.h"

#include "../log.h"

#include <math.h>

/*
 * The constants of java.util.Random's LCG:
 * state = (state * 0x5DEECE66D + 0xB) mod 2^48
 */
STATIC FINAL long long int multiplier = 0x5DEECE66D;
STATIC FINAL long long int mask       = ((long long int) 1 << 48) - 1;
STATIC FINAL long long int addend     = 0xB;


/* Constructor: installs the methods. */
PUBLIC void random_create(Random* this) {
    this->set_seed      = random_set_seed;
    this->next          = random_next;
    this->next_int      = random_next_int;
    this->next_float    = random_next_float;
    this->next_double   = random_next_double;
    this->next_gaussian = random_next_gaussian;
    this->next_boolean  = random_next_boolean;

    this->seed = 0;
    this->nextNextGaussian = 0.0f;
    this->haveNextNextGaussian = false;
}


/* Java's setSeed(): mixes the seed with the multiplier and masks to 48 bits. */
PUBLIC void random_set_seed(Random* this, int seed) {
    this->seed = (seed ^ multiplier) & mask;
    this->haveNextNextGaussian = false;
}


/* Core of the generator: one LCG step, returning the highest `bits` bits. */
PUBLIC int random_next(Random* this, int bits) {
    long long int seed = this->seed;

    this->seed = (seed * multiplier + addend) & mask;

    return (int) ((unsigned long long int) this->seed >> (48 - bits));
}


/* One random bit, as Java's nextBoolean(). */
PUBLIC boolean random_next_boolean(Random* this) {
    return this->next(this, 1) != 0;
}


/*
 * Uniform int in [0, bound). Powers of two take the fast path of multiply
 * and shift; everything else retries with rejection, exactly as Java's
 * nextInt() does, so the distribution stays unbiased and the sequence
 * stays identical.
 */
PUBLIC int random_next_int(Random* this, int bound) {
    int r = 0;
    int m = bound - 1;

    /*
     * Java: Random.nextInt(int bound) opens with
     *
     *     if (bound <= 0) throw new IllegalArgumentException(
     *             "bound must be positive");
     *
     * There is no exception to throw here, and the consequences of letting
     * a bad bound through are not mild: `u % bound` is a division by zero
     * for bound == 0 (SIGFPE on x86), and for a negative bound the loop
     * below can return a negative or out-of-range value, which every
     * caller then uses as a count, a damage figure or a coordinate.
     *
     * Java's contract is kept by clamping to the smallest legal bound and
     * saying so, which turns a hard crash into a visible warning.
     */
    if (bound <= 0) {
        LOG_WARN("nextInt(%d): bound must be positive, clamped to 1", bound);
        bound = 1;
        m = 0;
    }

    r = this->next(this, 31);

    /*
     * Java tests `(bound & -bound) == bound` to detect a power of two and
     * take the fast path. Here `bound & (bound - 1)` is the inverse test:
     * non-zero when bound is NOT a power of two, which is exactly when the
     * rejection loop is needed.
     */
    if (bound & m) {
        for (int u = r; u - (r = u % bound) + m < 0; u = this->next(this, 31)) {
            ;
        }

    } else {
        r = (int) ((bound * ((long long int) r)) >> 31);
    }

    return r;
}


/* Uniform float in [0, 1): 24 random bits scaled by 2^-24. */
PUBLIC float random_next_float(Random* this) {
    return this->next(this, 24) / (float) (1 << 24);
}


/* Uniform double in [0, 1): 53 random bits scaled by 2^-53. */
PUBLIC double random_next_double(Random* this) {
    return (((long long int) this->next(this, 26) << 27) + this->next(this, 27))
           * (1.0 / (((long long int) 1) << 53));
}


/*
 * Java's nextGaussian(): the polar method (Box-Muller), which produces the
 * samples two at a time. The second one is cached in the Random and handed
 * back on the next call, step for step as Java does it.
 */
PUBLIC float random_next_gaussian(Random* this) {
    if (this->haveNextNextGaussian) {
        this->haveNextNextGaussian = false;
        return this->nextNextGaussian;
    }

    float v1, v2, s;

    do {
        v1 = 2 * this->next_double(this) - 1;
        v2 = 2 * this->next_double(this) - 1;
        s  = (v1 * v1) + (v2 * v2);
    } while (s >= 1 || s == 0);

    float norm = sqrt(-2 * log(s) / s);

    this->nextNextGaussian     = v2 * norm;
    this->haveNextNextGaussian = true;

    return v1 * norm;
}
