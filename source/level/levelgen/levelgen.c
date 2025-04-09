#include <stdlib.h>
#include "../../utils/javarandom.h"
#include "levelgen.h"
#include "../../utils/utils.h"
#include "../tile/tileids.h"

#include <string.h>
#include <stdio.h>


static Random lg_random;


static inline void setSample(LevelGen* gen, int x, int y, double value) {
	gen->values[(x & (gen->w - 1)) + (y & (gen->h - 1)) * gen->w] = value;
}


static inline double sample(LevelGen* gen, int x, int y) {
	return gen->values[(x & (gen->w - 1)) + (y & (gen->h - 1)) * gen->w];
}


void levelgen_preinit() {
	random_set_seed(&lg_random, getTimeUS() / 1000);
}


void levelgen_free(LevelGen* gen) {
	free(gen->values);
}


void levelgen_init(LevelGen* gen, int w, int h, int featureSize) {
	gen->w = w;
	gen->h = h;

	gen->values = malloc(w * h * sizeof(double));
	memset(gen->values, 0, sizeof(double) * w * h);

	for (int y = 0; y < w; y += featureSize) {
		for (int x = 0; x < w; x += featureSize) {
			setSample(gen, x, y, random_next_float(&lg_random) * 2 - 1);
		}
	}

	int stepSize = featureSize;
	double scale = 1.0 / w;
	double scaleMod = 1;

	do {
		int halfStep = stepSize / 2;

		for (int y = 0; y < w; y += stepSize) {
			for (int x = 0; x < w; x += stepSize) {
				double a = sample(gen, x, y);
				double b = sample(gen, x + stepSize, y);
				double c = sample(gen, x, y + stepSize);
				double d = sample(gen, x + stepSize, y + stepSize);

				double e = (a + b + c + d) / 4.0 + (random_next_float(&lg_random) * 2 - 1) * stepSize * scale;
				setSample(gen, x + halfStep, y + halfStep, e);
			}
		}

		for (int y = 0; y < w; y += stepSize) {
			for (int x = 0; x < w; x += stepSize) {
				double a = sample(gen, x, y);
				double b = sample(gen, x + stepSize, y);
				double c = sample(gen, x, y + stepSize);
				double d = sample(gen, x + halfStep, y + halfStep);
				double e = sample(gen, x + halfStep, y - halfStep);
				double f = sample(gen, x - halfStep, y + halfStep);

				double H = (a + b + d + e) / 4.0 + (random_next_float(&lg_random) * 2 - 1) * stepSize * scale * 0.5;
				double g = (a + c + d + f) / 4.0 + (random_next_float(&lg_random) * 2 - 1) * stepSize * scale * 0.5;
				setSample(gen, x + halfStep, y, H);
				setSample(gen, x, y + halfStep, g);
			}
		}

		stepSize /= 2;
		scale *= (scaleMod + 0.8);
		scaleMod *= 0.3;

	} while (stepSize > 1);
}


void createUndergroundMap(unsigned char** map_r, unsigned char** data_r, int w, int h, int depth){
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

	unsigned char* map = malloc(w * h);
	unsigned char* data = malloc(w * h);
	memset(map, 0, w * h);
	memset(data, 0, w * h);

	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			int i = x + y * w;

			double val = dabs(noise1.values[i] - noise2.values[i]) * 3 - 2;

			double mval = dabs(mnoise1.values[i] - mnoise2.values[i]);
			mval = dabs(mval - mnoise3.values[i]) * 3 - 2;

			double nval = dabs(nnoise1.values[i] - nnoise2.values[i]);
			nval = dabs(nval - nnoise3.values[i]) * 3 - 2;

			double wval = dabs(wnoise1.values[i] - wnoise2.values[i]);
			wval = dabs(nval - wnoise3.values[i]) * 3 - 2; // Notch forgot to replace nval with wval?

			double xd = x / (w - 1.0) * 2 - 1;
			double yd = y / (h - 1.0) * 2 - 1;

			if (xd < 0) xd = -xd;
			if (yd < 0) yd = -yd;

			double dist = xd >= yd ? xd : yd;
			dist = dist * dist * dist * dist;
			dist = dist * dist * dist * dist;
			val = val + 1 - dist * 20;

			if (val > -2 && wval < -2.0 + (depth) / 2 * 3) {
				if(depth > 2) map[i] = LAVA;
				else map[i] = WATER;

			} else if(val > -2 && (mval < -1.7 || nval < -1.4)) {
				map[i] = DIRT;

			} else {
				map[i] = ROCK;
			}
		}
	}

	// I suppose Notch wanted to add another for loop here?
	int r = 2;
	for (int i = 0; i < w * h / 400; ++i) {
		int x = random_next_int(&lg_random, w);
		int y = random_next_int(&lg_random, h);

		for (int j = 0; j < 30; ++j) {
			int xx = x + random_next_int(&lg_random, 5) - random_next_int(&lg_random, 5);
			int yy = y + random_next_int(&lg_random, 5) - random_next_int(&lg_random, 5);

			if (xx >= r && yy >= r && xx < w - r && yy < h - r) {
				if (map[xx + yy * w] == ROCK) map[xx + yy * w] = IRON_ORE + depth - 1; // Kinda unsafe Notch
			}
		}
	}

	if (depth < 3) {
		int count = 0;

		for (int i = 0; i < w*h / 100; ++i) {
			int x = random_next_int(&lg_random, w-20) + 10;
			int y = random_next_int(&lg_random, h-20) + 10;

			for (int yy = y - 1; yy <= y + 1; ++yy) {
				for (int xx = x - 1; xx <= x + 1; ++xx) {
					if (map[xx + yy*w] != ROCK) goto cont_loop_1;
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


void createSkyMap(unsigned char** map_r, unsigned char** data_r, int w, int h){
	LevelGen noise1, noise2;

	levelgen_init(&noise1, w, h, 8);
	levelgen_init(&noise2, w, h, 8);

	unsigned char* map = malloc(w * h);
	unsigned char* data = malloc(w * h);

	memset(map, 0, w * h);
	memset(data, 0, w * h);

	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			int i = x + y*w;

			double val = dabs(noise1.values[i] - noise2.values[i]) * 3 - 2;

			double xd = x / (w - 1.0) * 2 - 1;
			double yd = y / (h - 1.0) * 2 - 1;

			if(xd < 0) xd = -xd;
			if(yd < 0) yd = -yd;

			double dist = xd >= yd ? xd : yd;
			dist = dist * dist * dist * dist;
			dist = dist * dist * dist * dist;
			val = -val * 1 - 2.2;
			val = val + 1 - dist * 20;

			if(val < -0.25) map[i] = INFINITE_FALL;
			else map[i] = CLOUD;
		}
	}

	for(int i = 0; i < w*h / 50; ++i){
		int x = random_next_int(&lg_random, w-2) + 1;
		int y = random_next_int(&lg_random, h-2) + 1;

		for (int yy = y-1; yy <= y + 1; ++yy) {
			for (int xx = x - 1; xx <= x + 1; ++xx) {
				if (map[xx + yy * w] != CLOUD) goto loop_cloudcacti_fail;
			}
		}

		map[x + y * w] = CLOUD_CACTUS;

		loop_cloudcacti_fail:
		continue;
	}

	int count = 0;
	for (int i = 0; i < w*h; ++i) {
		int x = random_next_int(&lg_random, w - 2) + 1;
		int y = random_next_int(&lg_random, h - 2) + 1;

		for (int yy = y - 1; yy <= y + 1; ++yy) {
			for (int xx = x - 1; xx <= x + 1; ++xx) {
				if (map[xx + yy * w] != CLOUD) goto loop_stairs_fail;
			}
		}

		map[x + y * w] = STAIRS_DOWN;
		++count;
		if(count == 2) break;

		loop_stairs_fail:
		continue;
	}


	*map_r = map;
	*data_r = data;
	levelgen_free(&noise1);
	levelgen_free(&noise2);
}


void createTopMap(unsigned char** map_r, unsigned char** data_r, int w, int h) {
	LevelGen mnoise1, mnoise2, mnoise3, noise1, noise2;

	levelgen_init(&mnoise1, w, h, 16);
	levelgen_init(&mnoise2, w, h, 16);
	levelgen_init(&mnoise3, w, h, 16);

	levelgen_init(&noise1, w, h, 32);
	levelgen_init(&noise2, w, h, 32);

	unsigned char* map = malloc(w * h);
	unsigned char* data = malloc(w * h);
	memset(map, 0, w*h);
	memset(data, 0, w*h);

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

	for (int i = 0; i < w * h / 2800; ++i) {
		int xs = random_next_int(&lg_random, w);
		int ys = random_next_int(&lg_random, h);

		for (int k = 0; k < 10; ++k) {
			int x = xs + random_next_int(&lg_random, 21) - 10;
			int y = ys + random_next_int(&lg_random, 21) - 10;

			for (int j = 0; j < 100; ++j) {
				int xo = x + random_next_int(&lg_random, 5) - random_next_int(&lg_random, 5);
				int yo = y + random_next_int(&lg_random, 5) - random_next_int(&lg_random, 5);

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


	for (int i = 0; i < w * h / 400; ++i) {
		int x = random_next_int(&lg_random, w);
		int y = random_next_int(&lg_random, h);

		for (int j = 0; j < 200; ++j) {
			int xx = x + random_next_int(&lg_random, 15) - random_next_int(&lg_random, 15);
			int yy = y + random_next_int(&lg_random, 15) - random_next_int(&lg_random, 15);

			if (xx >= 0 && yy >= 0 && xx < w && yy < h) {
				if (map[xx + yy * w] == GRASS) map[xx + yy * w] = TREE;
			}
		}
	}


	for (int i = 0; i < w * h / 400; ++i) {
		int x = random_next_int(&lg_random, w);
		int y = random_next_int(&lg_random, h);
		int col = random_next_int(&lg_random, 4);

		for (int j = 0; j < 30; ++j) {
			int xx = x + random_next_int(&lg_random, 5) - random_next_int(&lg_random, 5);
			int yy = y + random_next_int(&lg_random, 5) - random_next_int(&lg_random, 5);

			if (xx >= 0 && yy >= 0 && xx < w && yy < h) {
				if (map[xx + yy * w] == GRASS) {
					map[xx + yy * w] = FLOWER;
					data[xx + yy * w] = (unsigned char) (col + random_next_int(&lg_random, 4) * 16);
				}
			}
		}
	}

	for (int i = 0; i < w * h / 100; ++i) {
		int xx = random_next_int(&lg_random, w);
		int yy = random_next_int(&lg_random, h);

		if (xx >= 0 && yy >= 0 && xx < w && yy < h) {
			if (map[xx + yy * w] == SAND) map[xx + yy * w] = CACTUS;
		}
	}

	int count = 0;

	for (int i = 0; i < w * h / 100; ++i) {

		int x = random_next_int(&lg_random, w - 2) + 1;
		int y = random_next_int(&lg_random, h - 2) + 1;

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


void createAndValidateSkyMap(unsigned char** map_r, unsigned char** data_r, int w, int h) {
	int count[256];

	do{
		memset(count, 0, sizeof(int) * 256);
		createSkyMap(map_r, data_r, w, h);
		unsigned char* map = *map_r;

		for (int i = 0; i < w * h; ++i){
			++count[map[i]];
		}

		if (count[CLOUD] < 2000) goto genfail;
		if (count[STAIRS_DOWN] < 2) goto genfail;

		break;
		genfail:
		printf("Gen failed %d %d\n", count[CLOUD], count[STAIRS_DOWN]);

		free(map);
		free(*data_r);

	} while(1);
}


void createAndValidateUndergroundMap(unsigned char** map_r, unsigned char** data_r, int w, int h, int depth) {
	int count[256];

	do {
		memset(count, 0, sizeof(int) * 256);
		createUndergroundMap(map_r, data_r, w, h, depth);
		unsigned char* map = *map_r;

		for (int i = 0; i < w * h; ++i){
			++count[map[i]];
		}

		if (count[ROCK] < 100) goto genfail;
		if (count[DIRT] < 100) goto genfail;
		if (count[IRON_ORE + depth - 1] < 20) goto genfail; // Unsafe x2
		if (depth < 3) if (count[STAIRS_DOWN] < 2) continue;

		break;
		genfail:
		free(map);
		free(*data_r);
	} while(1);
}


void createAndValidateTopMap(unsigned char** map_r, unsigned char** data_r, int w, int h) {
	int count[256]; // TODO: maybe remove?

	do {
		memset(count, 0, sizeof(int) * 256);
		createTopMap(map_r, data_r, w, h);
		unsigned char* map = *map_r;

		for(int i = 0; i < w * h; ++i){
			++count[map[i]];
		}

		if (count[ROCK] < 100) goto genfail;
		if (count[SAND] < 100) goto genfail;
		if (count[GRASS] < 100) goto genfail;
		if (count[TREE] < 100) goto genfail;
		if (count[STAIRS_DOWN] < 2) goto genfail;

		break;
		genfail:
		free(map);
		free(*data_r);
	} while(1);
}
