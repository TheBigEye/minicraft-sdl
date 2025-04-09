#include "tile.h"
#include "../../item/resource/resource.h"
#include "../../gfx/color.h"
#include "../../gfx/color.h"
#include "../../item/item.h"
#include "../../entity/itementity.h"
#include "../../entity/particle/smashparticle.h"
#include "../../entity/particle/textparticle.h"
#include "../level.h"
#include "../../item/resourceitem.h"

void oretile_init(TileID id, Resource* toDrop){
	tile_init(id);
	Tile* t = tiles + id;
	t->add.ore.toDrop = toDrop;
	t->add.ore.color = toDrop->color & 0xffff00;
}

void oretile_render(TileID id, Screen* screen, Level* level, int x, int y){
	tiles[id].add.ore.color = (tiles[id].add.ore.toDrop->color & 0xffffff00) + getColor(level->dirtColor);

	render_screen(screen, x * 16 + 0, y * 16 + 0, 17 + 1 * 32, tiles[id].add.ore.color, 0);
	render_screen(screen, x * 16 + 8, y * 16 + 0, 18 + 1 * 32, tiles[id].add.ore.color, 0);
	render_screen(screen, x * 16 + 0, y * 16 + 8, 17 + 2 * 32, tiles[id].add.ore.color, 0);
	render_screen(screen, x * 16 + 8, y * 16 + 8, 18 + 2 * 32, tiles[id].add.ore.color, 0);
}


void oretile_hurt_(TileID id, Level* level, int x, int y, int dmg){
	int damage = level_get_data(level, x, y) + 1;

	SmashParticle* smash = malloc(sizeof(SmashParticle));
	smashparticle_create(smash, x*16 + 8, y*16+8);
	level_addEntity(level, (Entity *) smash);

	TextParticle* txt = malloc(sizeof(TextParticle));
	char* tx_ = malloc(16);
	sprintf(tx_, "%d\00", dmg);
	textparticle_create(txt, tx_, x*16 + 8, y*16 + 8, getColor4(-1, 500, 500, 500));
	level_addEntity(level, (Entity *) txt);
	Random* random = &tiles[id].random;
	if(dmg > 0){
		int count = random_next_int(random, 2);
		if(damage >= random_next_int(random, 10) + 3){
			level_set_tile(level, x, y, DIRT, 0);
			count += 2;
		}else{
			level_set_data(level, x, y, damage);
		}

		for(int i = 0; i < count; ++i){
			ItemEntity* entity = malloc(sizeof(ItemEntity));
			Item item;
			resourceitem_create(&item, tiles[id].add.ore.toDrop);
			itementity_create(entity, item, x*16 + random_next_int(random, 10) + 3, y*16 + random_next_int(random, 10) + 3);
			level_addEntity(level, (Entity *) entity);
		}
	}
}


char oretile_interact(TileID id, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
	if(item->id == TOOL){
		if(item->add.tool.type == PICKAXE){
			if(player_payStamina(player, 6 - item->add.tool.level)){
				oretile_hurt_(id, level, xt, yt, 1);
				return 1;
			}
		}
	}
	return 0;
}


void oretile_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir){
	oretile_hurt_(id, level, x, y, 0);
}
