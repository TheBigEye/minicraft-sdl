/*
 * sapling_tile.h - The sapling tile (Java: tile.SaplingTile).
 *
 * It grows on one kind of ground, `onType`, and when it matures it turns
 * into another tile, `growsTo`. It copies the ground's connection flags so
 * that its edges blend the same way.
 */
#ifndef LEVEL_TILE_SAPLING_TILE_H_
#define LEVEL_TILE_SAPLING_TILE_H_ 1

#include "tile.h"

#include "../level.h"
#include "../../entity/mob.h"

/* Constructor. Java: SaplingTile(int id, Tile onType, Tile growsTo) */
PUBLIC void saplingtile_init(Tile* this, TileID id, TileID onType, TileID growsTo);

/* Java: SaplingTile.render(Screen, Level, int, int) */
PUBLIC void saplingtile_render(Tile* this, Screen* screen, Level* level, int x, int y);

/* Java: SaplingTile.tick(Level, int, int) */
PUBLIC void saplingtile_tick(Tile* this, Level* level, int xt, int yt);

/* Java: SaplingTile.hurt(Level, int, int, Mob, int, int) */
PUBLIC void saplingtile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir);

#endif /* LEVEL_TILE_SAPLING_TILE_H_ */
