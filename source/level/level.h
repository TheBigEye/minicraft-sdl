/*
 * level.h - One game level: tile grid, per-tile data and the entities
 *           living on it (Java: com.mojang.ld22.level.Level).
 *
 * Five instances exist at once (sky, surface and three underground
 * layers), linked by stairs; `depth` selects the generation style.
 */
#ifndef LEVEL_LEVEL_H_
#define LEVEL_LEVEL_H_ 1

#include "../entity/entity.h"
#include "../gfx/screen.h"
#include "../utils/arraylist.h"
#include "../utils/javarandom.h"


typedef struct _Level{
	Random random;              /* generation RNG, seeded per level */
	int w, h;                   /* size in tiles */
	unsigned char* tiles;       /* tile id grid, w*h bytes */
	unsigned char* data;        /* per-tile data byte grid */
	ArrayList* entitiesInTiles; /* per-tile entity buckets */
	int grassColor;             /* palette index of grass here */
	int dirtColor;              /* palette index of dirt here */
	int sandColor;              /* palette index of sand here */
	int depth;                  /* +1 sky, 0 surface, <0 underground */
	int monsterDensity;         /* spawn spacing factor */
	ArrayList entities;         /* all entities on this level */
} Level;


/* Tries `count` random mob spawns respecting density rules. */
void level_trySpawn(Level* level, int count);
/* Generates the level for its depth and links stairs to `parent`. */
void level_init(Level* lvl, int w, int h, int level, Level* parent);
/* Tile id at (x, y); caller guarantees in-bounds coordinates. */
unsigned char level_get_tile(Level* level, int x, int y);
/* Per-tile data byte at (x, y) (growth stage, ore type, ...). */
unsigned char level_get_data(Level* level, int x, int y);
/* Draws the tile layer visible in the scroll window. */
void level_renderBackground(Level* level, Screen* screen, int xScroll, int yScroll);
/* Ticks every entity, drops removed ones, tries mob spawns. */
void level_tick(Level* level);
/* Replaces the tile at (x, y) and resets its data byte. */
void level_set_tile(Level* level, int x, int y, int id, int data);
/* Sets the data byte of the tile at (x, y). */
void level_set_data(Level* level, int x, int y, int val);
/* Draws the visible entities sorted bottom-to-top. */
void level_renderSprites(Level* level, Screen* screen, int xScroll, int yScroll);
/* Unregisters an entity from its tile bucket. */
void level_removeEntity(Level* level, int x, int y, Entity* entity);
/* Removes an entity from the level entity list. */
void level_removeEntity1(Level* level, Entity* e);
/* Adds an entity to the level (takes ownership of its memory). */
void level_addEntity(Level* level, Entity* entity);
/* Registers an entity in the bucket of the tile it stands on. */
void level_insertEntity(Level* level, int x, int y, Entity* entity);
/* Collects entities overlapping the given pixel rectangle. */
void level_getEntities(Level* level, ArrayList* list, int x0, int y0, int x1, int y1);
/* Builds and applies the darkness/light map for the view. */
void renderLight(Level* level, Screen* screen, int xScroll, int yScroll);

/* Releases tiles, data, buckets and the entity list. */
void level_free(Level* lvl);

#endif /* LEVEL_LEVEL_H_ */
