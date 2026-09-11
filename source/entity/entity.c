/*
 * entity.c - Base Entity behavior: the C equivalent of the Java
 *            com.mojang.ld22.entity.Entity class body.
 *
 * Holds the default virtual implementations every subclass vtable
 * inherits, plus the non-virtual methods Java never overrode
 * (init, intersects, move, move2, interact, remove).
 */
#include "entity.h"
#include <stdio.h>

#include "player.h"

#include "../item/item.h"
#include "../level/tile/tileids.h"
#include "../level/tile/tile.h"


/*
 * Initializes the common Entity fields. Subclass create() functions call
 * this first and then install their own vtable over entity_vtable.
 * The collision radius defaults to 6 pixels (the Java original's value).
 */
void entity_create(Entity* entity) {
	entity->vt = &entity_vtable; /* subclasses override with their own vtable */
	entity->x = entity->y = 0;
	entity->xr = entity->yr = 6;
	entity->removed = 0;
	entity->level = 0;
	random_set_seed(&entity->random, getTimeMS());
}


/*
 * Applies an item's interact behavior to this entity (e.g. hitting a mob
 * or tile-with-entity with a tool). Delegates to the item's own logic.
 */
uint8_t entity_interact(Entity* entity, Player* player, Item* item, int attackDir) {
	return item_interact(item, player, entity, attackDir);
}


/* Marks the entity for removal; the level drops it on the next pass. */
void entity_remove(Entity* entity) {
	entity->removed = 1;
}


/* ------------------------------------------------------------------ */
/* Default virtual implementations - the Java Entity base class body. */
/* Each one is the behavior a subclass inherits when its vtable does   */
/* not provide an override.                                            */
/* ------------------------------------------------------------------ */

/* Base tick(): does nothing; subclasses drive their own updates. */
void entity_tick(Entity* entity) {
	/* public void tick() {} */
	(void) entity;
}

/* Base render(): invisible by default; subclasses draw themselves. */
void entity_render(Entity* entity, Screen* screen) {
	/* public void render(Screen screen) {} */
	(void) entity;
	(void) screen;
}

/* Base blocks(): entities never block others; Mob and Furniture override. */
char entity_blocks(Entity* entity, Entity* other) {
	/* public boolean blocks(Entity e) { return false; } */
	(void) entity;
	(void) other;
	return 0;
}

/* Base hurt(): immune to mob attacks unless a subclass says otherwise. */
void entity_hurt(Entity* entity, Mob* source, int dmg, int attackDir) {
	/* public void hurt(Mob mob, int dmg, int attackDir) {} */
	(void) entity; (void) source; (void) dmg; (void) attackDir;
}

/* Base hurtTile(): immune to tile damage (mining, hoeing) by default. */
void entity_hurtTile(Entity* entity, TileID tile, int x, int y, int dmg) {
	/* public void hurt(Tile tile, int x, int y, int dmg) {} */
	(void) entity; (void) tile; (void) x; (void) y; (void) dmg;
}

/* Base touchedBy(): no reaction to being touched by another entity. */
void entity_touchedBy(Entity* entity, Entity* other) {
	/* protected void touchedBy(Entity entity) {} */
	(void) entity;
	(void) other;
}

/* Base isBlockableBy(): movement of any mob is blocked by this entity. */
char entity_isBlockableBy(Entity* entity, Mob* mob) {
	/* public boolean isBlockableBy(Mob mob) { return true; } */
	(void) entity;
	(void) mob;
	return 1;
}

/* Base touchItem(): ignores item pickups; Player overrides to collect. */
void entity_touchItem(Entity* entity, struct _ItemEntity* item) {
	/* public void touchItem(ItemEntity itemEntity) {} */
	(void) entity;
	(void) item;
}

/* Base canSwim(): cannot enter water; Player overrides. */
char entity_canSwim(Entity* entity) {
	/* public boolean canSwim() { return false; } */
	(void) entity;
	return 0;
}

/* Base use(): cannot be used by the player; Furniture overrides. */
char entity_use(Entity* entity, Player* player, int attackDir) {
	/* public boolean use(Player player, int attackDir) { return false; } */
	(void) entity; (void) player; (void) attackDir;
	return 0;
}

/* Base getLightRadius(): emits no light; Lantern overrides with 8. */
int entity_getLightRadius(Entity* entity) {
	/* public int getLightRadius() { return 0; } */
	(void) entity;
	return 0;
}

/* Base die(): unreachable in practice (only Mobs die); diagnostic only. */
void entity_die(Entity* entity) {
	/* Only Mobs can die in the original; keep the old diagnostic. */
	printf("Tried dying undyable entity (wat)! %d\n", entity->type);
}

/* Base doHurt(): same as die(), a diagnostic for logic bugs. */
void entity_doHurt(Entity* entity, int damage, int attackDir) {
	/* Only Mobs can be hurt in the original; keep the old diagnostic. */
	(void) damage; (void) attackDir;
	printf("Tried hurting unhurtable entity! %d\n", entity->type);
}

/* Base isSwimming(): Mob-only query; non-mobs are never swimming. */
char entity_isSwimming(Entity* entity) {
	/* Mob-only method; non-mobs never swim. */
	(void) entity;
	return 0;
}

/* Base free(): the plain Entity owns no heap resources. */
void entity_free(Entity* entity) {
	/* C-specific: default entities own no heap resources. */
	(void) entity;
}

/* The base vtable (= the Java `Entity` class). */
const EntityVTable entity_vtable = {
	.tick           = entity_tick,
	.render         = entity_render,
	.blocks         = entity_blocks,
	.hurt           = entity_hurt,
	.hurtTile       = entity_hurtTile,
	.touchedBy      = entity_touchedBy,
	.isBlockableBy  = entity_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = entity_use,
	.getLightRadius = entity_getLightRadius,
	.die            = entity_die,
	.doHurt         = entity_doHurt,
	.isSwimming     = entity_isSwimming,
	.free           = entity_free,
};


/* Attaches the entity to a level; called by level_addEntity(). */
void entity_init(Entity* entity, Level* level) {
	entity->level = level;
}


/*
 * True if the entity's bounding box (center +/- xr, yr) intersects the
 * given tile-space rectangle, i.e. the Java Entity.intersects() test.
 */
uint8_t entity_intersects(Entity* entity, int x0, int y0, int x1, int y1) {
	return !(entity->x + entity->xr < x0 || entity->y + entity->yr < y0 || entity->x - entity->xr > x1 || entity->y - entity->yr > y1);
}


/*
 * Single-axis movement with collision, the Java Entity.move2().
 *
 * Coordinates: entities live in pixel space (2048x2048) while tiles live
 * in tile space (128x128); each tile is 16x16 pixels, so ">> 4" converts
 * pixels to tiles and the tile range touched by the move is swept for
 * blocking tiles. Tiles entered are notified via bumpedInto(), then the
 * entities overlapping the destination are asked via touchedBy()/blocks()
 * whether they allow the move. On success the position is updated.
 */
uint8_t entity_move2(Entity* entity, int xa, int ya) {
	if (xa && ya) {
		printf("Entity(%d) called move2 with xa and ya != 0!\n", entity->type);
	}

	/* Tile box currently occupied... */
	int xto0 = (entity->x - entity->xr) >> 4;
	int yto0 = (entity->y - entity->yr) >> 4;
	int xto1 = (entity->x + entity->xr) >> 4;
	int yto1 = (entity->y + entity->yr) >> 4;

	/* ...and tile box occupied after the move. */
	int xt0 = (entity->x + xa - entity->xr) >> 4;
	int yt0 = (entity->y + ya - entity->yr) >> 4;
	int xt1 = (entity->x + xa + entity->xr) >> 4;
	int yt1 = (entity->y + ya + entity->yr) >> 4;

	uint8_t blocked = 0;

	/* Sweep the destination box; tiles already occupied are skipped. */
	for (int yt = yt0; yt <= yt1; ++yt) {
		for (int xt = xt0; xt <= xt1; ++xt) {

			if (xt >= xto0 && xt <= xto1 && yt >= yto0 && yt <= yto1) {
                continue;
            }

			TileID tile = level_get_tile(entity->level, xt, yt);
			tile_bumpedInto(tile, entity->level, xt, yt, entity);
			tile = level_get_tile(entity->level, xt, yt); // Reget tile in case bumpedInto changed it?
			if (!tile_mayPass(tile, entity->level, xt, yt, entity)) {
				blocked = 1; // Not needed?
				return 0;
			}
		}
	}

	if (blocked) return 0;
	ArrayList wasInside, isInside;
	create_arraylist(&wasInside);
	create_arraylist(&isInside);

	int x = entity->x;
	int xr = entity->xr;
	int y = entity->y;
	int yr = entity->yr;

	/* Entities overlapped before and after the move. */
	level_getEntities(entity->level, &wasInside, x - xr, y - yr, x + xr, y + yr);
	level_getEntities(entity->level, &isInside, x + xa - xr, y + ya - yr, x + xa + xr, y + ya + yr);

	/* Notify newly touched entities (ItemEntity pickup, AirWizard, ...). */
	for (int i = 0; i < isInside.size; ++i) {
		Entity* e = isInside.elements[i];
		if (e == entity) continue;
		e->vt->touchedBy(e, entity);   /* Java: e.touchedBy(this) */
	}

	/* Entities already overlapping before the move cannot block it. */
	for (int j = 0; j < wasInside.size; ++j) {
		for (int i = 0; i < isInside.size; ++i) {
			Entity* a = isInside.elements[i];
			Entity* b = wasInside.elements[j];

			if (a == b) {
                arraylist_removeId(&isInside, i--);
            }
		}
	}

	arraylist_remove(&wasInside);

	/* Any remaining overlapping entity that blocks us stops the move. */
	for (int i = 0; i < isInside.size; ++i) {
		Entity* e = isInside.elements[i];
		if (e == entity) continue;

		if (e->vt->blocks(e, entity)) {   /* Java: e.blocks(this) */
			arraylist_remove(&isInside);
			return 0;
		}
	}

	arraylist_remove(&isInside);
	entity->x += xa;
	entity->y += ya;

	return 1;
}


/*
 * Full movement step, the Java Entity.move(): diagonal movement is split
 * into two single-axis move2() passes so collisions stay axis-aligned.
 * When the entity ends up on a new tile, that tile is notified via
 * steppedOn() (saplings, flowers, stairs...). Returns success.
 */
uint8_t entity_move(Entity* entity, int xa, int ya) {
	if (xa || ya) {
		uint8_t stopped = 1;

		if (xa != 0 && entity_move2(entity, xa, 0)) stopped = 0;
		if (ya != 0 && entity_move2(entity, 0, ya)) stopped = 0;

		if (!stopped) {
			int xt = entity->x >> 4;
			int yt = entity->y >> 4;

			uint8_t tile = level_get_tile(entity->level, xt, yt);
			tile_steppedOn(tile, entity->level, xt, yt, entity);
		}

		return !stopped;
	}

	return 1;
}
