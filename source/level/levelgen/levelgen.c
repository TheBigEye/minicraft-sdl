/*
 * levelgen.c - Level generation (Java: com.mojang.ld22.level.levelgen.LevelGen).
 *
 * Terrain is built from diamond-square noise fields: several fields are
 * combined, pushed away from the map border and thresholded into tile ids;
 * then scattered feature passes add ores, stairs, sand, trees, flowers,
 * cacti and cloud cacti. The validating wrappers regenerate the whole map
 * until its tile census looks playable.
 */
#include "levelgen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../tile/tileids.h"
#include "../../log.h"
#include "../../utils/javarandom.h"
#include "../../utils/utils.h"

/*
 * The generator shared by every pass.
 * Java: `private static final Random random = new Random()`.
 *
 * It cannot be called `random` here: stdlib.h declares a function of that
 * name, so a global variable with it would be a redeclaration.
 */
STATIC Random lg_random;

/* A 2D field of noise samples, produced by the diamond-square pass. */
struct LevelGen {
    /* Noise samples, indexed x + y * w. */
    double* values;
    /* Field width, a power of two. */
    int w;
    /* Field height, a power of two. */
    int h;
};


/* Reads a noise sample, wrapping the coordinates around the field.
 * Java: private double sample(int x, int y) */
PRIVATE double sample(LevelGen* this, int x, int y) {
    return this->values[(x & (this->w - 1)) + (y & (this->h - 1)) * this->w];
}


/* Writes a noise sample, wrapping the coordinates around the field.
 * Java: private void setSample(int x, int y, double value) */
PRIVATE void set_sample(LevelGen* this, int x, int y, double value) {
    this->values[(x & (this->w - 1)) + (y & (this->h - 1)) * this->w] = value;
}


/*
 * Seeds the shared generation RNG from the current millisecond time.
 *
 * `lg_random` is a static object, so its methods have to be installed
 * before it can be used; in Java the field initialiser `new Random()` did
 * that by itself.
 */
PUBLIC void levelgen_preinit(void) {
    random_create(&lg_random);
    lg_random.set_seed(&lg_random, get_time_us() / 1000);
}


/* Releases a noise field's sample buffer. Java left this to the GC. */
PRIVATE void levelgen_free(LevelGen* this) {
    delete(this->values);
}


/*
 * Fills a new noise field with the diamond-square algorithm: seeds a
 * coarse grid at featureSize spacing, then repeatedly interpolates centres
 * and edge midpoints with a shrinking step size and jitter.
 * Java: public LevelGen(int w, int h, int featureSize)
 */
PRIVATE void levelgen_init(LevelGen* this, int w, int h, int featureSize) {
    this->w = w;
    this->h = h;

    this->values = new_array(double, w * h);

    for (int y = 0; y < w; y += featureSize) {
        for (int x = 0; x < w; x += featureSize) {
            set_sample(this, x, y, lg_random.next_float(&lg_random) * 2 - 1);
        }
    }

    int stepSize = featureSize;
    double scale = 1.0 / w;
    double scaleMod = 1;

    do {
        int halfStep = stepSize / 2;

        /* Diamond step: the centre of each square. */
        for (int y = 0; y < w; y += stepSize) {
            for (int x = 0; x < w; x += stepSize) {
                double a = sample(this, x, y);
                double b = sample(this, x + stepSize, y);
                double c = sample(this, x, y + stepSize);
                double d = sample(this, x + stepSize, y + stepSize);

                double e = (a + b + c + d) / 4.0
                        + (lg_random.next_float(&lg_random) * 2 - 1) * stepSize * scale;

                set_sample(this, x + halfStep, y + halfStep, e);
            }
        }

        /* Square step: the midpoint of each edge. */
        for (int y = 0; y < w; y += stepSize) {
            for (int x = 0; x < w; x += stepSize) {
                double a = sample(this, x, y);
                double b = sample(this, x + stepSize, y);
                double c = sample(this, x, y + stepSize);
                double d = sample(this, x + halfStep, y + halfStep);
                double e = sample(this, x + halfStep, y - halfStep);
                double f = sample(this, x - halfStep, y + halfStep);

                double H = (a + b + d + e) / 4.0
                        + (lg_random.next_float(&lg_random) * 2 - 1) * stepSize * scale * 0.5;
                double g = (a + c + d + f) / 4.0
                        + (lg_random.next_float(&lg_random) * 2 - 1) * stepSize * scale * 0.5;

                set_sample(this, x + halfStep, y, H);
                set_sample(this, x, y + halfStep, g);
            }
        }

        stepSize /= 2;
        scale *= (scaleMod + 0.8);
        scaleMod *= 0.3;

    } while (stepSize > 1);
}


/*
 * Generates an underground level of the given depth, 1 to 3: rock with
 * dirt and water, or lava below depth 2, from combined noise fields; then
 * it scatters ore blobs and, above depth 3, up to four down-stairs.
 * Java: private static byte[][] createUndergroundMap(int w, int h, int depth)
 */
PRIVATE void create_underground_map(unsigned char** map_r, unsigned char** data_r, int w, int h, int depth) {
    LevelGen mnoise1, mnoise2, mnoise3;
    LevelGen nnoise1, nnoise2, nnoise3;
    LevelGen wnoise1, wnoise2, wnoise3;
    LevelGen noise1, noise2;

    levelgen_init(&mnoise1, w, h, 16);
    levelgen_init(&mnoise2, w, h, 16);
    levelgen_init(&mnoise3, w, h, 16);

    levelgen_init(&nnoise1, w, h, 16);
    levelgen_init(&nnoise2, w, h, 16);
    levelgen_init(&nnoise3, w, h, 16);

    levelgen_init(&wnoise1, w, h, 16);
    levelgen_init(&wnoise2, w, h, 16);
    levelgen_init(&wnoise3, w, h, 16);

    levelgen_init(&noise1, w, h, 32);
    levelgen_init(&noise2, w, h, 32);

    unsigned char* map = new_array(unsigned char, w * h);
    unsigned char* data = new_array(unsigned char, w * h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int i = x + y * w;

            double val = dabs(noise1.values[i] - noise2.values[i]) * 3 - 2;

            double mval = dabs(mnoise1.values[i] - mnoise2.values[i]);
            mval = dabs(mval - mnoise3.values[i]) * 3 - 2;

            double nval = dabs(nnoise1.values[i] - nnoise2.values[i]);
            nval = dabs(nval - nnoise3.values[i]) * 3 - 2;

            double wval = dabs(wnoise1.values[i] - wnoise2.values[i]);
            /* Notch forgot to replace nval with wval here; kept as is. */
            wval = dabs(nval - wnoise3.values[i]) * 3 - 2;

            double xd = x / (w - 1.0) * 2 - 1;
            double yd = y / (h - 1.0) * 2 - 1;

            if (xd < 0) xd = -xd;
            if (yd < 0) yd = -yd;

            double dist = xd >= yd ? xd : yd;

            dist = dist * dist * dist * dist;
            dist = dist * dist * dist * dist;
            val = val + 1 - dist * 20;

            if (val > -2 && wval < -2.0 + depth / 2 * 3) {
                map[i] = depth > 2 ? LAVA : WATER;
            } else if (val > -2 && (mval < -1.7 || nval < -1.4)) {
                map[i] = DIRT;
            } else {
                map[i] = ROCK;
            }
        }
    }

    /* I suppose Notch wanted to add another for loop here? */
    int r = 2;

    for (int i = 0; i < w * h / 400; ++i) {
        int x = lg_random.next_int(&lg_random, w);
        int y = lg_random.next_int(&lg_random, h);

        for (int j = 0; j < 30; ++j) {
            int xx = x + lg_random.next_int(&lg_random, 5) - lg_random.next_int(&lg_random, 5);
            int yy = y + lg_random.next_int(&lg_random, 5) - lg_random.next_int(&lg_random, 5);

            if (xx >= r && yy >= r && xx < w - r && yy < h - r) {
                /* Kinda unsafe, Notch: it assumes the three ores are consecutive ids. */
                if (map[xx + yy * w] == ROCK) map[xx + yy * w] = IRON_ORE + depth - 1;
            }
        }
    }

    if (depth < 3) {
        int count = 0;

        for (int i = 0; i < w * h / 100; ++i) {
            int x = lg_random.next_int(&lg_random, w - 20) + 10;
            int y = lg_random.next_int(&lg_random, h - 20) + 10;

            for (int yy = y - 1; yy <= y + 1; ++yy) {
                for (int xx = x - 1; xx <= x + 1; ++xx) {
                    if (map[xx + yy * w] != ROCK) goto cont_loop_1;
                }
            }

            map[x + y * w] = STAIRS_DOWN;
            ++count;

            if (count == 4) break;

        cont_loop_1:
            continue;
        }
    }

    *map_r = map;
    *data_r = data;

    levelgen_free(&mnoise1);
    levelgen_free(&mnoise2);
    levelgen_free(&mnoise3);

    levelgen_free(&nnoise1);
    levelgen_free(&nnoise2);
    levelgen_free(&nnoise3);

    levelgen_free(&wnoise1);
    levelgen_free(&wnoise2);
    levelgen_free(&wnoise3);

    levelgen_free(&noise1);
    levelgen_free(&noise2);
}


/*
 * Generates the sky level: cloud islands floating over an infinite fall,
 * plus scattered cloud cacti and two down-stairs.
 * Java: private static byte[][] createSkyMap(int w, int h)
 */
PRIVATE void create_sky_map(unsigned char** map_r, unsigned char** data_r, int w, int h) {
    LevelGen noise1, noise2;

    levelgen_init(&noise1, w, h, 8);
    levelgen_init(&noise2, w, h, 8);

    unsigned char* map = new_array(unsigned char, w * h);
    unsigned char* data = new_array(unsigned char, w * h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int i = x + y * w;

            double val = dabs(noise1.values[i] - noise2.values[i]) * 3 - 2;

            double xd = x / (w - 1.0) * 2 - 1;
            double yd = y / (h - 1.0) * 2 - 1;

            if (xd < 0) xd = -xd;
            if (yd < 0) yd = -yd;

            double dist = xd >= yd ? xd : yd;

            dist = dist * dist * dist * dist;
            dist = dist * dist * dist * dist;
            val = -val * 1 - 2.2;
            val = val + 1 - dist * 20;

            map[i] = val < -0.25 ? INFINITE_FALL : CLOUD;
        }
    }

    for (int i = 0; i < w * h / 50; ++i) {
        int x = lg_random.next_int(&lg_random, w - 2) + 1;
        int y = lg_random.next_int(&lg_random, h - 2) + 1;

        for (int yy = y - 1; yy <= y + 1; ++yy) {
            for (int xx = x - 1; xx <= x + 1; ++xx) {
                if (map[xx + yy * w] != CLOUD) goto loop_cloudcacti_fail;
            }
        }

        map[x + y * w] = CLOUD_CACTUS;

    loop_cloudcacti_fail:
        continue;
    }

    int count = 0;

    for (int i = 0; i < w * h; ++i) {
        int x = lg_random.next_int(&lg_random, w - 2) + 1;
        int y = lg_random.next_int(&lg_random, h - 2) + 1;

        for (int yy = y - 1; yy <= y + 1; ++yy) {
            for (int xx = x - 1; xx <= x + 1; ++xx) {
                if (map[xx + yy * w] != CLOUD) goto loop_stairs_fail;
            }
        }

        map[x + y * w] = STAIRS_DOWN;
        ++count;

        if (count == 2) break;

    loop_stairs_fail:
        continue;
    }

    *map_r = map;
    *data_r = data;

    levelgen_free(&noise1);
    levelgen_free(&noise2);
}


/*
 * Generates the overworld: water, rock and grass from noise, then feature
 * passes for sand beaches, trees, flowers with their colour and shape
 * data, cacti on sand and up to four down-stairs in rock.
 * Java: private static byte[][] createTopMap(int w, int h)
 */
PRIVATE void create_top_map(unsigned char** map_r, unsigned char** data_r, int w, int h) {
    LevelGen mnoise1, mnoise2, mnoise3, noise1, noise2;

    levelgen_init(&mnoise1, w, h, 16);
    levelgen_init(&mnoise2, w, h, 16);
    levelgen_init(&mnoise3, w, h, 16);

    levelgen_init(&noise1, w, h, 32);
    levelgen_init(&noise2, w, h, 32);

    unsigned char* map = new_array(unsigned char, w * h);
    unsigned char* data = new_array(unsigned char, w * h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int i = x + y * w;

            double val = dabs(noise1.values[i] - noise2.values[i]) * 3 - 2;

            double mval = dabs(mnoise1.values[i] - mnoise2.values[i]);
            mval = dabs(mval - mnoise3.values[i]) * 3 - 2;

            double xd = x / (w - 1.0) * 2 - 1;
            double yd = y / (h - 1.0) * 2 - 1;

            if (xd < 0) xd = -xd;
            if (yd < 0) yd = -yd;

            double dist = xd >= yd ? xd : yd;

            dist = dist * dist * dist * dist;
            dist = dist * dist * dist * dist;
            val = val + 1 - dist * 20;

            if (val < -0.5) map[i] = WATER;
            else if (val > 0.5 && mval < -1.5) map[i] = ROCK;
            else map[i] = GRASS;
        }
    }

    /* Sand beaches: a handful of blobs scattered around random spots. */
    for (int i = 0; i < w * h / 2800; ++i) {
        int xs = lg_random.next_int(&lg_random, w);
        int ys = lg_random.next_int(&lg_random, h);

        for (int k = 0; k < 10; ++k) {
            int x = xs + lg_random.next_int(&lg_random, 21) - 10;
            int y = ys + lg_random.next_int(&lg_random, 21) - 10;

            for (int j = 0; j < 100; ++j) {
                int xo = x + lg_random.next_int(&lg_random, 5) - lg_random.next_int(&lg_random, 5);
                int yo = y + lg_random.next_int(&lg_random, 5) - lg_random.next_int(&lg_random, 5);

                for (int yy = yo - 1; yy <= yo + 1; ++yy) {
                    for (int xx = xo - 1; xx <= xo + 1; ++xx) {
                        if (xx >= 0 && yy >= 0 && xx < w && yy < h) {
                            if (map[xx + yy * w] == GRASS) map[xx + yy * w] = SAND;
                        }
                    }
                }
            }
        }
    }

    /* Forests. */
    for (int i = 0; i < w * h / 400; ++i) {
        int x = lg_random.next_int(&lg_random, w);
        int y = lg_random.next_int(&lg_random, h);

        for (int j = 0; j < 200; ++j) {
            int xx = x + lg_random.next_int(&lg_random, 15) - lg_random.next_int(&lg_random, 15);
            int yy = y + lg_random.next_int(&lg_random, 15) - lg_random.next_int(&lg_random, 15);

            if (xx >= 0 && yy >= 0 && xx < w && yy < h) {
                if (map[xx + yy * w] == GRASS) map[xx + yy * w] = TREE;
            }
        }
    }

    /* Flower fields: the data byte carries the colour and the shape. */
    for (int i = 0; i < w * h / 400; ++i) {
        int x = lg_random.next_int(&lg_random, w);
        int y = lg_random.next_int(&lg_random, h);
        int col = lg_random.next_int(&lg_random, 4);

        for (int j = 0; j < 30; ++j) {
            int xx = x + lg_random.next_int(&lg_random, 5) - lg_random.next_int(&lg_random, 5);
            int yy = y + lg_random.next_int(&lg_random, 5) - lg_random.next_int(&lg_random, 5);

            if (xx >= 0 && yy >= 0 && xx < w && yy < h) {
                if (map[xx + yy * w] == GRASS) {
                    map[xx + yy * w] = FLOWER;
                    data[xx + yy * w] = (unsigned char) (col + lg_random.next_int(&lg_random, 4) * 16);
                }
            }
        }
    }

    /* Cacti, one per sandy square it manages to hit. */
    for (int i = 0; i < w * h / 100; ++i) {
        int xx = lg_random.next_int(&lg_random, w);
        int yy = lg_random.next_int(&lg_random, h);

        if (xx >= 0 && yy >= 0 && xx < w && yy < h) {
            if (map[xx + yy * w] == SAND) map[xx + yy * w] = CACTUS;
        }
    }

    int count = 0;

    for (int i = 0; i < w * h / 100; ++i) {
        int x = lg_random.next_int(&lg_random, w - 2) + 1;
        int y = lg_random.next_int(&lg_random, h - 2) + 1;

        for (int yy = y - 1; yy <= y + 1; ++yy) {
            for (int xx = x - 1; xx <= x + 1; ++xx) {
                if (map[xx + yy * w] != ROCK) goto cont_loop_1;
            }
        }

        map[x + y * w] = STAIRS_DOWN;
        ++count;

        if (count == 4) break;

    cont_loop_1:
        continue;
    }

    *map_r = map;
    *data_r = data;

    levelgen_free(&mnoise1);
    levelgen_free(&mnoise2);
    levelgen_free(&mnoise3);
    levelgen_free(&noise1);
    levelgen_free(&noise2);
}


/*
 * Regenerates the sky map until it has enough cloud and at least two
 * down-stairs, freeing every failed attempt.
 * Java: public static byte[][] createAndValidateSkyMap(int w, int h)
 */
PUBLIC void create_and_validate_sky_map(unsigned char** map_r, unsigned char** data_r, int w, int h) {
    int count[256];

    do {
        memset(count, 0, sizeof(int) * 256);

        create_sky_map(map_r, data_r, w, h);

        unsigned char* map = *map_r;

        for (int i = 0; i < w * h; ++i) {
            ++count[map[i]];
        }

        if (count[CLOUD] < 2000) goto genfail;
        if (count[STAIRS_DOWN] < 2) goto genfail;

        break;

    genfail:
        LOG_TRACE("Sky gen failed: cloud=%d stairsDown=%d", count[CLOUD], count[STAIRS_DOWN]);

        delete(map);
        delete(*data_r);

    } while (1);
}


/*
 * Regenerates the underground map until the rock, dirt and ore census and
 * the down-stairs count are acceptable for the depth.
 * Java: public static byte[][] createAndValidateUndergroundMap(int, int, int)
 */
PUBLIC void create_and_validate_underground_map(unsigned char** map_r, unsigned char** data_r, int w, int h, int depth) {
    int count[256];

    do {
        memset(count, 0, sizeof(int) * 256);

        create_underground_map(map_r, data_r, w, h, depth);

        unsigned char* map = *map_r;

        for (int i = 0; i < w * h; ++i) {
            ++count[map[i]];
        }

        if (count[ROCK] < 100) goto genfail;
        if (count[DIRT] < 100) goto genfail;
        /* Unsafe x2: it assumes the three ores are consecutive ids. */
        if (count[IRON_ORE + depth - 1] < 20) goto genfail;
        if (depth < 3 && count[STAIRS_DOWN] < 2) goto genfail;

        break;

    genfail:
        delete(map);
        delete(*data_r);

    } while (1);
}


/*
 * Regenerates the overworld map until every major tile type and the
 * down-stairs are present in sufficient quantity.
 * Java: public static byte[][] createAndValidateTopMap(int w, int h)
 */
PUBLIC void create_and_validate_top_map(unsigned char** map_r, unsigned char** data_r, int w, int h) {
    int count[256];

    do {
        memset(count, 0, sizeof(int) * 256);

        create_top_map(map_r, data_r, w, h);

        unsigned char* map = *map_r;

        for (int i = 0; i < w * h; ++i) {
            ++count[map[i]];
        }

        if (count[ROCK] < 100) goto genfail;
        if (count[SAND] < 100) goto genfail;
        if (count[GRASS] < 100) goto genfail;
        if (count[TREE] < 100) goto genfail;
        if (count[STAIRS_DOWN] < 2) goto genfail;

        break;

    genfail:
        delete(map);
        delete(*data_r);

    } while (1);
}
