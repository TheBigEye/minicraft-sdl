/*
 * slime.c - Slime mob (Java: entity.Slime).
 *
 * A simple hopping enemy: jumps in random directions, harms the
 * player on contact and drops slime resources when killed.
 */
#include "slime.h"
#include <stdio.h>
#include <stdlib.h>
#include "mob.h"
#include "../game.h"
#include "itementity.h"
#include "../item/resourceitem.h"
#include "../item/resource/resource.h"
#include "../gfx/screen.h"
#include "../gfx/color.h"

/* The Slime vtable (= the Java `Slime` class). */
static const EntityVTable slime_vtable = {
	.tick           = (vt_tick_fn) slime_tick,
	.render         = (vt_render_fn) slime_render,
	.blocks         = (vt_blocks_fn) mob_blocks,
	.hurt           = (vt_hurt_fn) mob_hurt,
	.hurtTile       = (vt_hurtTile_fn) mob_hurtTile,
	.touchedBy      = (vt_touchedBy_fn) slime_touchedBy,
	.isBlockableBy  = entity_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = entity_use,
	.getLightRadius = entity_getLightRadius,
	.die            = (vt_die_fn) slime_die,
	.doHurt         = (vt_doHurt_fn) mob_doHurt,
	.isSwimming     = (vt_isSwimming_fn) mob_isSwimming,
	.free           = entity_free,
};


/* Spawns a slime of level `lvl` at a random spot; health and
 * damage scale with the level (health = lvl*lvl*5). */
void slime_create(Slime* slime, int lvl) {
	mob_create(&slime->mob);
	slime->mob.entity.vt = &slime_vtable;

	if (lvl <= 0) printf("WAT\n");

	slime->mob.entity.type = SLIME;
	slime->mob.entity.x = random_next_int(&slime->mob.entity.random, 64 * 16);
	slime->mob.entity.y = random_next_int(&slime->mob.entity.random, 64 * 16);
	slime->mob.health = slime->mob.maxHealth = lvl * lvl * 5;
	slime->lvl = lvl;
	slime->jumpTime = 0;
	slime->xa = slime->ya = 0;
}


/* Hopping AI: keeps jumping along the current direction and picks
 * a new random one when blocked or by chance, as in Java. */
void slime_tick(Slime* slime){
	mob_tick(&slime->mob);
	Random* random = &slime->mob.entity.random;

	int speed = 1;

	if (!mob_move(&slime->mob, slime->xa * speed, slime->ya * speed) || random_next_int(random, 40) == 0) {
		if (slime->jumpTime <= -10) {
			slime->xa = random_next_int(random, 3) - 1;
			slime->ya = random_next_int(random, 3) - 1;

			if (game_player->mob.entity.level == slime->mob.entity.level) {
				int xd = game_player->mob.entity.x - slime->mob.entity.x;
				int yd = game_player->mob.entity.y - slime->mob.entity.y;

				if (((xd * xd) + (yd * yd)) < (50 * 50)) {
					if (xd < 0) slime->xa = -1;
					if (xd > 0) slime->xa = 1;
					if (yd < 0) slime->ya = -1;
					if (yd > 0) slime->ya = 1;
				}

			}

			if (slime->xa != 0 || slime->ya != 0) slime->jumpTime = 10;
		}
	}

	--slime->jumpTime;
	if (slime->jumpTime == 0) {
		slime->xa = slime->ya = 0;
	}
}


/* Drops 1-2 slime resource items around the corpse and awards
 * 25*lvl score when the player shares the level. */
void slime_die(Slime* slimee) {
	mob_die(&slimee->mob);

	Random* random = &slimee->mob.entity.random;
	int count = random_next_int(random, 2) + 1;

	for (int i = 0; i < count; ++i) {
		ItemEntity* item_entity = malloc(sizeof(ItemEntity));
        if (!item_entity) {
            continue;
        }

        Item resource;
		resourceitem_create(&resource, &slime);
		itementity_create(
            item_entity,
            resource,
            slimee->mob.entity.x + random_next_int(random, 11) - 5,
            slimee->mob.entity.y + random_next_int(random, 11) - 5
        );

        // level_addEntity takes ownership of the memory
		level_addEntity(slimee->mob.entity.level, &item_entity->entity);
	}

	if (game_player->mob.entity.level == slimee->mob.entity.level) {
		game_player->score += 25 * slimee->lvl;
	}
}


/* Draws the slime body, squashed while a jump is active. */
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


void slime_touchedBy(Slime* slime, struct _Entity* entity) {
	if (entity->type == PLAYER) {
		entity->vt->hurt(entity, &slime->mob, slime->lvl, slime->mob.dir);
	}
}
