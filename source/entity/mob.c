#include "mob.h"
#include "../game.h"
#include "../gfx/color.h"
#include "../level/tile/tileids.h"
#include "_entity_caller.h"
#include "entity.h"
#include "particle/textparticle.h"


void mob_create(Mob* mob){
	entity_create(&mob->entity);

	mob->walkDist = mob->dir = mob->hurtTime = 0;
	mob->xKnockback = mob->yKnockback = 0;
	mob->maxHealth = 10;
	mob->health = mob->maxHealth;
	mob->swimTimer = mob->tickTime = 0;

	mob->entity.x = mob->entity.y = 8;
	mob->entity.xr = 4;
	mob->entity.yr = 3;
}


void mob_doHurt(Mob* mob, int damage, int attackDir) {
	if (mob->hurtTime > 0) return;

	if (game_player->mob.entity.level == mob->entity.level) {
		/* TODO: Sounds
		 * int xd = level.player.x - x;
			int yd = level.player.y - y;
			if (xd * xd + yd * yd < 80 * 80) {
				Sound.monsterHurt.play();
			}
		*/
	}

	TextParticle* text_particle = malloc(sizeof(TextParticle));
	char* text = malloc(16);
	sprintf(text, "%d", damage);
	textparticle_create(text_particle, text, mob->entity.x, mob->entity.y, getColor4(-1, 500, 500, 500));
	level_addEntity(mob->entity.level, &text_particle->entity);

	mob->health -= damage;

	if (attackDir == 0) mob->yKnockback = 6;
	if (attackDir == 1) mob->yKnockback = -6;
	if (attackDir == 2) mob->xKnockback = -6;
	if (attackDir == 3) mob->xKnockback = 6;

	mob->hurtTime = 10;
}


void mob_hurt(Mob* mob, Mob* by, int damage, int attackDir) {
	call_entity_doHurt(&mob->entity, damage, attackDir);
}


void mob_hurtTile(Mob* mob, TileID tile, int x, int y, int damage) {
	int attackDir = mob->dir ^ 1;
	call_entity_doHurt(&mob->entity, damage, attackDir);
}


char mob_findStartPos(Mob* mob, Level* level) {
	Random* random = &mob->entity.random;
	int x = random_next_int(random, level->w);
	int y = random_next_int(random, level->h);

	int xx = (x * 16) + 8;
	int yy = (y * 16) + 8;

	if (game_player->mob.entity.level == mob->entity.level) {
		int xd = game_player->mob.entity.x - xx;
		int yd = game_player->mob.entity.y - yy;

		if (((xd * xd) + (yd * yd)) < 80 * 80) {
            return 0;
        }
	}

	int r = level->monsterDensity * 16;

	ArrayList ents;
	create_arraylist(&ents);

	level_getEntities(level, &ents, xx - r, yy - r, xx + r, yy + r);
	int sz = ents.size;
	arraylist_remove(&ents);
	if (sz > 0) return 0;

	TileID id = level_get_tile(level, x, y);
	if (tile_mayPass(id, level, x, y, &mob->entity)) {
		mob->entity.x = xx;
		mob->entity.y = yy;
		return 1;
	}

	return 0;
}


void mob_die(Mob* mob){
	entity_remove(&mob->entity);
}


void mob_tick(Mob* mob) {
	++mob->tickTime;

	if (level_get_tile(mob->entity.level, mob->entity.x >> 4, mob->entity.y >> 4) == LAVA) {
		mob_hurt(mob, mob, 4, mob->dir ^ 1);
	}

	if (mob->health <= 0) {
		call_entity_die(&mob->entity);
	}

	if (mob->hurtTime > 0) {
        --mob->hurtTime;
    }

}


uint8_t mob_isSwimming(Mob* mob) {
	TileID tile = level_get_tile(mob->entity.level, mob->entity.x >> 4, mob->entity.y >> 4);
	return tile == WATER || tile == LAVA;
}


void mob_heal(Mob* mob, int heal){
	if (mob->hurtTime > 0) {
        return;
    }

	char* text = malloc(16);
	sprintf(text, "%d", heal);

    TextParticle* text_particle = malloc(sizeof(TextParticle));
	textparticle_create(text_particle, text, mob->entity.x, mob->entity.y, getColor4(-1, 50, 50, 50));
	level_addEntity(mob->entity.level, &text_particle->entity);
	mob->health += heal;

	if (mob->health > mob->maxHealth) {
        mob->health = mob->maxHealth;
    }
}


uint8_t mob_move(Mob* mob, int xa, int ya) {
	if (call_entity_isSwimming(&mob->entity)) {
		if (mob->swimTimer++ % 2 == 0) {
            return 1;
        }
	}

	if (mob->xKnockback < 0) {
		entity_move2(&mob->entity, -1, 0);
		++mob->xKnockback;
	}

	if (mob->xKnockback > 0) {
		entity_move2(&mob->entity, 1, 0);
		--mob->xKnockback;
	}

	if (mob->yKnockback < 0) {
		entity_move2(&mob->entity, 0, -1);
		++mob->yKnockback;
	}

	if (mob->yKnockback > 0) {
		entity_move2(&mob->entity, 0, 1);
		--mob->yKnockback;
	}

	if (mob->hurtTime > 0) return 1;

	if (xa != 0 || ya != 0) {
		++mob->walkDist;
		if (xa < 0) mob->dir = 2;
		if (xa > 0) mob->dir = 3;
		if (ya < 0) mob->dir = 1;
		if (ya > 0) mob->dir = 0;
	}

	return entity_move(&mob->entity, xa, ya);
}
