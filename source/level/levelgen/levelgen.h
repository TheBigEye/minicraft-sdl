/*
 * levelgen.h - Level generation (Java: LevelGen): diamond-square
 *              noise fields turned into validated tile maps for the
 *              overworld, underground and sky levels.
 */
#ifndef LEVEL_LEVELGEN_LEVELGEN_H_
#define LEVEL_LEVELGEN_LEVELGEN_H_ 1

/* A 2D field of noise samples produced by the diamond-square pass. */
typedef struct{
	double* values; /* Noise samples, indexed x + y * w. */
	int w;          /* Field width (power of two). */
	int h;          /* Field height (power of two). */
} LevelGen;

/* Seeds the shared generation RNG from the clock; call once. */
void levelgen_preinit();

/* Generates the overworld map, retrying until it passes validation. */
void createAndValidateTopMap(unsigned char** map_r, unsigned char** data_r, int w, int h);
/* Generates an underground map of the given depth, retrying until valid. */
void createAndValidateUndergroundMap(unsigned char** map_r, unsigned char** data_r, int w, int h, int depth);
/* Generates the sky map, retrying until it passes validation. */
void createAndValidateSkyMap(unsigned char** map_r, unsigned char** data_r, int w, int h);

#endif /* LEVEL_LEVELGEN_LEVELGEN_H_ */
