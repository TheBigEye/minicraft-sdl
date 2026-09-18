/*
 * level.h - The Level class (Java: com.mojang.ld22.level.Level).
 *
 * A level is the tile grid, the per-tile data and the entities living in
 * it. Five exist at once (sky, surface and three depths), linked by
 * stairs; `depth` decides how the level is generated.
 *
 * The struct stays visible in the header because game.c holds the five
 * levels in a plain array, not through pointers.
 *
 * The methods mirror Level.java, so the code reads the same:
 *
 *     Tile* t = level->get_tile(level, x, y);         // level.getTile(x, y)
 *     level->set_tile(level, x, y, tiles[DIRT], 0);    // level.setTile(x, y, Tile.dirt, 0)
 *     level->add(level, entity);                       // level.add(entity)
 */
#ifndef LEVEL_LEVEL_H_
#define LEVEL_LEVEL_H_ 1

#include "../entity/entity.h"
#include "../gfx/screen.h"
#include "../utils/arraylist.h"
#include "../utils/javarandom.h"
#include "tile/tile.h"
#include "../utils/javalang.h"

typedef struct Level Level;

/* Signatures of the Level methods. */
typedef void          (*level_render_background_fn)(Level* this, Screen* screen, int xScroll, int yScroll);
typedef void          (*level_render_sprites_fn)   (Level* this, Screen* screen, int xScroll, int yScroll);
typedef void          (*level_render_light_fn)     (Level* this, Screen* screen, int xScroll, int yScroll);
typedef void          (*level_sort_and_render_fn)  (Level* this, Screen* screen, ArrayList* list);
typedef Tile*         (*level_get_tile_fn)         (Level* this, int x, int y);
typedef void          (*level_set_tile_fn)         (Level* this, int x, int y, Tile* t, int dataVal);
typedef unsigned char (*level_get_data_fn)         (Level* this, int x, int y);
typedef void          (*level_set_data_fn)         (Level* this, int x, int y, int val);
typedef void          (*level_add_fn)              (Level* this, Entity* entity);
typedef void          (*level_remove_fn)           (Level* this, Entity* e);
typedef void          (*level_insert_entity_fn)    (Level* this, int x, int y, Entity* entity);
typedef void          (*level_remove_entity_fn)    (Level* this, int x, int y, Entity* entity);
typedef void          (*level_get_entities_fn)     (Level* this, ArrayList* list, int x0, int y0, int x1, int y1);
typedef void          (*level_try_spawn_fn)        (Level* this, int count);
typedef void          (*level_tick_fn)             (Level* this);
typedef void          (*level_free_fn)             (Level* this);

struct Level {
    /* --- methods, installed by level_create() --- */

    /* Draws the visible tile layer. Java: renderBackground */
    level_render_background_fn render_background;
    /* Draws the visible entities, sorted by Y. Java: renderSprites */
    level_render_sprites_fn render_sprites;
    /* Builds and applies the light map. Java: renderLight */
    level_render_light_fn render_light;
    /* Sorts and draws one row of entities. Java: sortAndRender (private) */
    level_sort_and_render_fn sort_and_render;
    /* The tile at (x, y). Java: getTile */
    level_get_tile_fn get_tile;
    /* Replaces the tile and resets its data byte. Java: setTile */
    level_set_tile_fn set_tile;
    /* The per-tile data byte. Java: getData */
    level_get_data_fn get_data;
    /* Sets the data byte without touching the tile. Java: setData */
    level_set_data_fn set_data;
    /* Adds an entity; the level owns it from here on. Java: add */
    level_add_fn add;
    /* Removes an entity from the list and its bucket. Java: remove */
    level_remove_fn remove;
    /* Puts an entity in the bucket of the tile it stands on. Java: insertEntity (private) */
    level_insert_entity_fn insert_entity;
    /* Takes an entity out of a tile's bucket. Java: removeEntity (private) */
    level_remove_entity_fn remove_entity;
    /* Entities overlapping the given rectangle. Java: getEntities */
    level_get_entities_fn get_entities;
    /* Attempts `count` spawns, respecting the density. Java: trySpawn */
    level_try_spawn_fn try_spawn;
    /* One simulation tick. Java: tick */
    level_tick_fn tick;
    /* C destructor: frees the grids, the buckets and the entity list. */
    level_free_fn free;

    /* --- data --- */

    /* This level's own generator, seeded by create(). Java: private Random random */
    Random random;
    /* Size in tiles. Java: public int w, h */
    int w, h;
    /* Grid of tile ids, w*h bytes. Java: public byte[] tiles */
    unsigned char* tiles;
    /* Grid of per-tile data. Java: public byte[] data */
    unsigned char* data;
    /* One entity bucket per tile. Java: public List<Entity>[] entitiesInTiles */
    ArrayList* entitiesInTiles;
    /* Palette index of grass here. Java: public int grassColor = 141 */
    int grassColor;
    /* Palette index of dirt here. Java: public int dirtColor = 322 */
    int dirtColor;
    /* Palette index of sand here. Java: public int sandColor = 550 */
    int sandColor;
    /* +1 sky, 0 surface, negative underground. Java: private int depth */
    int depth;
    /* Spacing between spawn attempts. Java: public int monsterDensity = 8 */
    int monsterDensity;
    /* Every entity in the level. Java: public List<Entity> entities */
    ArrayList entities;
};

/*
 * Constructor (Java: Level(int w, int h, int level, Level parentLevel)).
 * Generates the level for its depth and links the stairs to its parent.
 *
 * Java names the third parameter `level` and stores it in the field
 * `depth`; this port calls it `depth` throughout, so that a reader never
 * has to wonder whether `level` means the depth or the object.
 *
 * Java also keeps a `public Player player` reference here. This port uses
 * the game_player global instead, so the field does not exist.
 */
PUBLIC void level_create(Level* this, int w, int h, int depth, Level* parent);

/* The method implementations. */
PUBLIC void          level_render_background(Level* this, Screen* screen, int xScroll, int yScroll);
PUBLIC void          level_render_sprites(Level* this, Screen* screen, int xScroll, int yScroll);
PUBLIC void          level_render_light(Level* this, Screen* screen, int xScroll, int yScroll);
PUBLIC void          level_sort_and_render(Level* this, Screen* screen, ArrayList* list);
PUBLIC Tile*         level_get_tile(Level* this, int x, int y);
PUBLIC void          level_set_tile(Level* this, int x, int y, Tile* t, int dataVal);
PUBLIC unsigned char level_get_data(Level* this, int x, int y);
PUBLIC void          level_set_data(Level* this, int x, int y, int val);
PUBLIC void          level_add(Level* this, Entity* entity);
PUBLIC void          level_remove(Level* this, Entity* e);
PUBLIC void          level_insert_entity(Level* this, int x, int y, Entity* entity);
PUBLIC void          level_remove_entity(Level* this, int x, int y, Entity* entity);
PUBLIC void          level_get_entities(Level* this, ArrayList* list, int x0, int y0, int x1, int y1);
PUBLIC void          level_try_spawn(Level* this, int count);
PUBLIC void          level_tick(Level* this);
PUBLIC void          level_free(Level* this);

#endif /* LEVEL_LEVEL_H_ */
