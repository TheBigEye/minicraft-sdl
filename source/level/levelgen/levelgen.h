/*
 * levelgen.h - Level generation (Java: com.mojang.ld22.level.levelgen.LevelGen).
 *
 * Diamond-square noise fields are combined, pushed away from the map border
 * and thresholded into tile ids; then scattered feature passes add ores,
 * stairs, sand, trees, flowers, cacti and cloud cacti. The validating
 * wrappers regenerate the whole map until its tile census looks playable.
 *
 * LevelGen is opaque here: only levelgen.c ever builds a noise field, so
 * the struct stays in the .c and this header only publishes the three
 * entry points the game calls.
 */
#ifndef LEVEL_LEVELGEN_LEVELGEN_H_
#define LEVEL_LEVELGEN_LEVELGEN_H_ 1

#include "../../utils/javalang.h"

/* Java: public class LevelGen. Its body is private to levelgen.c. */
typedef struct LevelGen LevelGen;

/*
 * Seeds the shared generation RNG from the clock. Call once, before any
 * map is generated.
 *
 * Java has no counterpart: `private static final Random random = new
 * Random()` is a field initialiser that runs on its own, whereas in C the
 * method pointers have to be installed by hand.
 */
PUBLIC void levelgen_preinit(void);

/*
 * Generates the overworld map, retrying until it validates.
 * Java: public static byte[][] createAndValidateTopMap(int w, int h)
 *
 * `map_r` and `data_r` are out parameters: Java returned the two rows of
 * the `byte[][]` instead.
 */
PUBLIC void create_and_validate_top_map(unsigned char** map_r, unsigned char** data_r, int w, int h);

/* Generates an underground map of the given depth, retrying until valid.
 * Java: public static byte[][] createAndValidateUndergroundMap(int, int, int) */
PUBLIC void create_and_validate_underground_map(unsigned char** map_r, unsigned char** data_r, int w, int h, int depth);

/* Generates the sky map, retrying until it validates.
 * Java: public static byte[][] createAndValidateSkyMap(int w, int h) */
PUBLIC void create_and_validate_sky_map(unsigned char** map_r, unsigned char** data_r, int w, int h);

#endif /* LEVEL_LEVELGEN_LEVELGEN_H_ */
