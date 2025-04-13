#include "entity.h"

#include "_entity_caller.h"

#include "player.h"

#include "../item/item.h"
#include "../level/tile/tileids.h"
#include "../level/tile/tile.h"


void entity_create(Entity* entity) {
	entity->x = entity->y = 0;
	entity->xr = entity->yr = 6;
	entity->removed = 0;
	entity->level = 0;
	random_set_seed(&entity->random, getTimeMS());
}


uint8_t entity_interact(Entity* entity, Player* player, Item* item, int attackDir) {
	return item_interact(item, player, entity, attackDir);
}


uint8_t entity_canSwim(Entity* entity) {
	return 0;
}


void entity_render(Entity* entity, Screen* screen) {
	// Useless here
}


void entity_remove(Entity* entity) {
	entity->removed = 1;
}


int entity_getLightRadius(Entity* entity) {
	return 0;
}


void entity_init(Entity* entity, Level* level) {
	entity->level = level;
}


uint8_t entity_intersects(Entity* entity, int x0, int y0, int x1, int y1) {
	return !(entity->x + entity->xr < x0 || entity->y + entity->yr < y0 || entity->x - entity->xr > x1 || entity->y - entity->yr > y1);
}


uint8_t entity_move2(Entity* entity, int xa, int ya) {
	if (xa && ya) {
		printf("Entity(%d) called move2 with xa and ya != 0!\n", entity->type);
	}

	int xto0 = (entity->x - entity->xr) >> 4;
	int yto0 = (entity->y - entity->yr) >> 4;
	int xto1 = (entity->x + entity->xr) >> 4;
	int yto1 = (entity->y + entity->yr) >> 4;

	int xt0 = (entity->x + xa - entity->xr) >> 4;
	int yt0 = (entity->y + ya - entity->yr) >> 4;
	int xt1 = (entity->x + xa + entity->xr) >> 4;
	int yt1 = (entity->y + ya + entity->yr) >> 4;

	uint8_t blocked = 0;

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

	level_getEntities(entity->level, &wasInside, x - xr, y - yr, x + xr, y + yr);
	level_getEntities(entity->level, &isInside, x + xa - xr, y + ya - yr, x + xa + xr, y + ya + yr);

	for (int i = 0; i < isInside.size; ++i) {
		Entity* e = isInside.elements[i];
		if (e == entity) continue;
		call_entity_touchedBy(e, entity);
	}

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

	for (int i = 0; i < isInside.size; ++i) {
		Entity* e = isInside.elements[i];
		if (e == entity) continue;

		if (call_entity_blocks(e, entity)) {
			arraylist_remove(&isInside);
			return 0;
		}
	}

	arraylist_remove(&isInside);
	entity->x += xa;
	entity->y += ya;

	return 1;
}


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
