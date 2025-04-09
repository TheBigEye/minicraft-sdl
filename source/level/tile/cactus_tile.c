#include "tile.h"
#include "../../gfx/color.h"
#include "../../entity/particle/smashparticle.h"
#include "../../entity/particle/textparticle.h"
#include "../../entity/itementity.h"
#include "../../item/resourceitem.h"

void cactustile_init(TileID id){
	tile_init(id);
	tiles[id].connectsToSand = 1;
}

void cactustile_render(TileID id, Screen* screen, Level* level, int x, int y){
	int col = getColor4(20, 40, 50, level->sandColor);

	render_screen(screen, x*16 + 0, y * 16 + 0, 8 + 2 * 32, col, 0);
	render_screen(screen, x*16 + 8, y * 16 + 0, 9 + 2 * 32, col, 0);
	render_screen(screen, x*16 + 0, y * 16 + 8, 8 + 3 * 32, col, 0);
	render_screen(screen, x*16 + 8, y * 16 + 8, 9 + 3 * 32, col, 0);
}



void cactus_hurt(TileID id, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
	int damage = level_get_data(level, x, y) + dmg;
	Random* random = &tiles[id].random;
	SmashParticle* smash = malloc(sizeof(SmashParticle));
	smashparticle_create(smash, x * 16 + 8, y*16+8);
	level_addEntity(level, (Entity *) smash);

	TextParticle* txt = malloc(sizeof(TextParticle));
	char* tx_ = malloc(16);
	sprintf(tx_, "%d\00", dmg);
	textparticle_create(txt, tx_, x*16 + 8, y*16 + 8, getColor4(-1, 500, 500, 500));
	level_addEntity(level, (Entity *) txt);

	if(damage >= 10){
		int count = random_next_int(random, 2) + 1;
		for(int i = 0; i < count; ++i){
			ItemEntity* ent = malloc(sizeof(ItemEntity));
			Item res;
			resourceitem_create(&res, &cactusFlower);
			int xx = x * 16 + random_next_int(random, 10) + 3;
			int yy = y * 16 + random_next_int(random, 10) + 3;
			itementity_create(ent, res, xx, yy);
			level_addEntity(level, (Entity *) ent);
		}
		level_set_tile(level, x, y, SAND, 0);
	}else{
		level_set_data(level, x, y, damage);
	}
}


void cactustile_tick(TileID id, Level* level, int xt, int yt){
	int damage = level_get_data(level, xt, yt);
	if(damage) level_set_data(level, xt, yt, damage - 1);
}
