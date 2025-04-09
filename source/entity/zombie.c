#include "../gfx/screen.h"
#include "_entity_caller.h"
#include "zombie.h"
#include "itementity.h"
#include "../item/resourceitem.h"
#include "../item/resource/resource.h"
#include "../game.h"
#include "../gfx/color.h"


void zombie_create(Zombie* zombie, int lvl){
	mob_create((Mob *) zombie);

	zombie->mob.entity.type = ZOMBIE;
	zombie->lvl = lvl;
	zombie->mob.entity.x = random_next_int(&zombie->mob.entity.random, 64*16);
	zombie->mob.entity.y = random_next_int(&zombie->mob.entity.random, 64*16);
	zombie->mob.health = zombie->mob.maxHealth = lvl*lvl*10;

	zombie->randomWalkTime = 0;
	zombie->xa = zombie->ya = 0;
}


void zombie_tick(Zombie* zombie){
	mob_tick((Mob *) zombie);

	Random* random = &zombie->mob.entity.random;

	if(game_player->mob.entity.level == zombie->mob.entity.level && zombie->randomWalkTime == 0){
		int xd = game_player->mob.entity.x - zombie->mob.entity.x;
		int yd = game_player->mob.entity.y - zombie->mob.entity.y;

		if(xd*xd + yd*yd < 50*50){
			zombie->ya = zombie->xa = 0;

			if(xd < 0) zombie->xa = -1;
			if(xd > 0) zombie->xa = 1;
			if(yd < 0) zombie->ya = -1;
			if(yd > 0) zombie->ya = 1;
		}
	}

	int speed = zombie->mob.tickTime & 1;
	if (!mob_move((Mob *) zombie, zombie->xa*speed, zombie->ya*speed) || random_next_int(random, 200) == 0) {
		zombie->randomWalkTime = 60;
		zombie->xa = (random_next_int(random, 3) - 1) * random_next_int(random, 2);
		zombie->ya = (random_next_int(random, 3) - 1) * random_next_int(random, 2);
	}

	if (zombie->randomWalkTime > 0) --zombie->randomWalkTime;
}


void zombie_render(Zombie* zombie, Screen* screen){
	int xt = 0;
	int yt = 14;

	int flip1 = (zombie->mob.walkDist >> 3) & 1;
	int flip2 = (zombie->mob.walkDist >> 3) & 1;

	if(zombie->mob.dir == 1) xt += 2;
	if(zombie->mob.dir > 1){
		flip1 = 0;
		flip2 = ((zombie->mob.walkDist >> 4) & 1);
		if (zombie->mob.dir == 2) {
			flip1 = 1;
		}
		xt += 4 + ((zombie->mob.walkDist >> 3) & 1) * 2;
	}

	int xo = zombie->mob.entity.x - 8;
	int yo = zombie->mob.entity.y - 11;

	int col = getColor4(-1, 10, 252, 050);
	if(zombie->lvl == 2) col = getColor4(-1, 100, 522, 050);
	if(zombie->lvl == 3) col = getColor4(-1, 111, 444, 050);
	if(zombie->lvl == 4) col = getColor4(-1, 000, 111, 020);

	if(zombie->mob.hurtTime > 0) col = getColor4(-1, 555, 555, 555);

	render_screen(screen, xo + 8 * flip1, yo + 0, xt + yt * 32, col, flip1);
	render_screen(screen, xo + 8 - 8 * flip1, yo + 0, xt + 1 + yt * 32, col, flip1);
	render_screen(screen, xo + 8 * flip2, yo + 8, xt + (yt + 1) * 32, col, flip2);
	render_screen(screen, xo + 8 - 8 * flip2, yo + 8, xt + 1 + (yt + 1) * 32, col, flip2);
}


void zombie_touchedBy(Zombie* zombie, Entity* entity){
	if(entity->type == PLAYER){
		call_entity_hurt(entity, (Mob *) zombie, zombie->lvl + 1, zombie->mob.dir);
	}
}


void zombie_die(Zombie* zombie){
	mob_die((Mob *) zombie);

	Random* random = &zombie->mob.entity.random;
	int count = random_next_int(random, 2) + 1;
	for(int i = 0; i < count; ++i){
		ItemEntity* item = malloc(sizeof(ItemEntity));
		Item resitem;
		resourceitem_create(&resitem, &cloth);
		itementity_create(item, resitem, zombie->mob.entity.x + random_next_int(random, 11) - 5, zombie->mob.entity.y + random_next_int(random, 11) - 5);
		level_addEntity(zombie->mob.entity.level, (Entity *) item);
	}

	if(game_player->mob.entity.level == zombie->mob.entity.level){
		game_player->score += 50*zombie->lvl;
	}
}
