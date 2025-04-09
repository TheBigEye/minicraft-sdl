#include "slime.h"
#include "mob.h"
#include "../game.h"
#include "itementity.h"
#include "../item/resourceitem.h"
#include "../item/resource/resource.h"
#include "../gfx/screen.h"
#include "../gfx/color.h"
#include "_entity_caller.h"


void slime_create(Slime* slime, int lvl){
	mob_create((Mob *) slime);

	if(lvl <= 0) printf("WAT\n");

	slime->mob.entity.type = SLIME;
	slime->mob.entity.x = random_next_int(&slime->mob.entity.random, 64*16);
	slime->mob.entity.y = random_next_int(&slime->mob.entity.random, 64*16);
	slime->mob.health = slime->mob.maxHealth = lvl*lvl*5;
	slime->lvl = lvl;
	slime->jumpTime = 0;
	slime->xa = slime->ya = 0;
}


void slime_tick(Slime* slime){
	mob_tick((Mob *) slime);
	Random* random = &slime->mob.entity.random;

	int speed = 1;

	if(!mob_move((Mob *) slime, slime->xa * speed, slime->ya * speed) || random_next_int(random, 40) == 0) {
		if (slime->jumpTime <= -10){
			slime->xa = random_next_int(random, 3) - 1;
			slime->ya = random_next_int(random, 3) - 1;

			if(game_player->mob.entity.level == slime->mob.entity.level){
				int xd = game_player->mob.entity.x - slime->mob.entity.x;
				int yd = game_player->mob.entity.y - slime->mob.entity.y;

				if(xd*xd + yd*yd < 50*50){
					if(xd < 0) slime->xa = -1;
					if(xd > 0) slime->xa = 1;
					if(yd < 0) slime->ya = -1;
					if(yd > 0) slime->ya = 1;
				}

			}

			if(slime->xa != 0 || slime->ya != 0) slime->jumpTime = 10;
		}
	}

	--slime->jumpTime;
	if (slime->jumpTime == 0) {
		slime->xa = slime->ya = 0;
	}

}


void slime_die(Slime* slimee){
	mob_die((Mob *) slimee);

	Random* random = &slimee->mob.entity.random;
	int count = random_next_int(random, 2) + 1;

	for (int i = 0; i < count; ++i) {
		ItemEntity* item = malloc(sizeof(ItemEntity));
		Item resitem;
		resourceitem_create(&resitem, &slime);
		itementity_create(item, resitem, slimee->mob.entity.x + random_next_int(random, 11) - 5, slimee->mob.entity.y + random_next_int(random, 11) - 5);
		level_addEntity(slimee->mob.entity.level, (Entity *) item);
	}

	if (game_player->mob.entity.level == slimee->mob.entity.level) {
		game_player->score += 25 * slimee->lvl;
	}
}


void slime_render(Slime* slime, Screen* screen){
	int xt = 0;
	int yt = 18;

	int xo = slime->mob.entity.x - 8;
	int yo = slime->mob.entity.y - 11;

	if (slime->jumpTime > 0) {
		xt += 2;
		yo -= 4;
	}

	int col = getColor4(-1, 10, 252, 555);
	if (slime->lvl == 2) col = getColor4(-1, 100, 522, 555);
	if (slime->lvl == 3) col = getColor4(-1, 111, 444, 555);
	if (slime->lvl == 4) col = getColor4(-1, 000, 111, 224);

	if (slime->mob.hurtTime > 0) {
		col = getColor4(-1, 555, 555, 555);
	}

	render_screen(screen, xo + 0, yo + 0, xt + yt * 32, col, 0);
	render_screen(screen, xo + 8, yo + 0, xt + 1 + yt * 32, col, 0);
	render_screen(screen, xo + 0, yo + 8, xt + (yt + 1) * 32, col, 0);
	render_screen(screen, xo + 8, yo + 8, xt + 1 + (yt + 1) * 32, col, 0);

}
void slime_touchedBy(Slime* slime, struct _Entity* entity){
	if(entity->type == PLAYER){
		call_entity_hurt(entity, (Mob *) slime, slime->lvl, slime->mob.dir);
	}
}
