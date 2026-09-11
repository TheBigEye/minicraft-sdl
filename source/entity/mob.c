/*
 * mob.c - Shared behavior of all living entities (Java: Mob class body).
 *
 * Health bookkeeping, knockback, swimming, spawn placement and the
 * hurt/die flow live here; concrete mobs (Player, Slime, Zombie,
 * AirWizard) override only what their Java classes override.
 */
#include "mob.h"
#include <stdio.h>
#include <stdlib.h>
#include "../game.h"
#include "../gfx/color.h"
#include "../level/tile/tileids.h"
#include "entity.h"
#include "particle/textparticle.h"
#include "../sound/sound.h"


/* The Mob vtable (= the Java `Mob` class). Mob subclasses copy these
 * entries and override only what they override in Java. */
const EntityVTable mob_vtable = {
	.tick           = (vt_tick_fn) mob_tick,
	.render         = entity_render,   /* abstract in Java; concrete mobs always override */
	.blocks         = (vt_blocks_fn) mob_blocks,
	.hurt           = (vt_hurt_fn) mob_hurt,
	.hurtTile       = (vt_hurtTile_fn) mob_hurtTile,
	.touchedBy      = entity_touchedBy,
	.isBlockableBy  = entity_isBlockableBy,
	.touchItem      = entity_touchItem,
	.canSwim        = entity_canSwim,
	.use            = entity_use,
	.getLightRadius = entity_getLightRadius,
	.die            = (vt_die_fn) mob_die,
	.doHurt         = (vt_doHurt_fn) mob_doHurt,
	.isSwimming     = (vt_isSwimming_fn) mob_isSwimming,
	.free           = entity_free,
};


/* Base mob initialization; subclasses call this then set their vtable. */
void mob_create(Mob* mob){
	entity_create(&mob->entity);
	mob->entity.vt = &mob_vtable; /* subclasses override with their own vtable */

	mob->walkDist = mob->dir = mob->hurtTime = 0;
	mob->xKnockback = mob->yKnockback = 0;
	mob->maxHealth = 10;
	mob->health = mob->maxHealth;
	mob->swimTimer = mob->tickTime = 0;

	mob->entity.x = mob->entity.y = 8;
	mob->entity.xr = 4;
	mob->entity.yr = 3;
}


/* Class-tag test for the four Mob subclasses (Java: instanceof Mob). */
char entity_ismob(Entity* entity) {
	/* The C equivalent of Java's `e instanceof Mob` checks. */
	switch (entity->type) {
		case SLIME:
		case ZOMBIE:
		case AIRWIZARD:
		case PLAYER:
			return 1;
		default:
			return 0;
	}
}


/*
 * Applies damage to the mob (Java: Mob.doHurt). While the hurt cooldown
 * runs the mob is invulnerable. Plays monsterHurt when the player is
 * within 80 pixels, spawns a red damage number, subtracts health and
 * queues knockback opposite to the attack direction.
 */
void mob_doHurt(Mob* mob, int damage, int attackDir) {
	if (mob->hurtTime > 0) return;

	if (game_player->mob.entity.level == mob->entity.level) {
		/* Sound.monsterHurt.play() - only if the player is close by
		 * (original Java: Mob.doHurt, distance check vs level.player) */
		int xd = game_player->mob.entity.x - mob->entity.x;
		int yd = game_player->mob.entity.y - mob->entity.y;
		if (xd * xd + yd * yd < 80 * 80) {
			sound_play(SND_MONSTERHURT);
		}
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


/* Entry point for mob-on-mob damage; dispatches to the virtual doHurt
 * so Player's override (invulnerability frames, death) applies. */
void mob_hurt(Mob* mob, Mob* by, int damage, int attackDir) {
	(void) by;
	/* Java: doHurt() is virtual - Player overrides it. */
	mob->entity.vt->doHurt(&mob->entity, damage, attackDir);
}


/* Damage dealt by the tile the mob stands on (lava, cactus...); the
 * attack direction is the opposite of the mob's facing, as in Java. */
void mob_hurtTile(Mob* mob, TileID tile, int x, int y, int damage) {
	(void) tile; (void) x; (void) y;
	int attackDir = mob->dir ^ 1;
	mob->entity.vt->doHurt(&mob->entity, damage, attackDir);
}


/* Collision query: whether `entity` stops this mob's movement. The
 * decision is delegated to the other entity, mirroring Java. */
char mob_blocks(Mob* mob, Entity* entity) {
	/* Java: Mob.blocks(e) { return e.isBlockableBy(this); } */
	return entity->vt->isBlockableBy(entity, mob);
}


/*
 * Tries one random spawn position (Java: Mob.findStartPos): rejects
 * spots within 80 pixels of the player and spots whose neighborhood
 * (monsterDensity tiles around) already holds entities, then requires
 * the tile itself to let this mob pass. Returns success; the caller
 * (level spawning) retries until it succeeds.
 */
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


/* Base death: just removes the entity; Player and AirWizard override
 * it to drop loot, play sounds or end the game. */
void mob_die(Mob* mob){
	entity_remove(&mob->entity);
}


/*
 * Per-tick update shared by all mobs (Java: Mob.tick): lava under the
 * mob hurts it, zero or less health triggers the virtual die(), and
 * the hurt invulnerability cooldown decays.
 */
void mob_tick(Mob* mob) {
	++mob->tickTime;

	if (level_get_tile(mob->entity.level, mob->entity.x >> 4, mob->entity.y >> 4) == LAVA) {
		mob_hurt(mob, mob, 4, mob->dir ^ 1);
	}

	if (mob->health <= 0) {
		mob->entity.vt->die(&mob->entity);   /* Java: die() is virtual */
	}

	if (mob->hurtTime > 0) {
        --mob->hurtTime;
    }

}


/* Swimming test: the tile under the mob's center is water or lava. */
uint8_t mob_isSwimming(Mob* mob) {
	TileID tile = level_get_tile(mob->entity.level, mob->entity.x >> 4, mob->entity.y >> 4);
	return tile == WATER || tile == LAVA;
}


/* Heals the mob (not while recently hurt), shows a green number popup
 * and clamps health at maxHealth, as Java's Mob.heal(). */
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


/*
 * Mob movement (Java: Mob.move): swimming halves the effective speed
 * (every other tick is skipped), pending knockback is applied one
 * pixel at a time, hurt mobs cannot walk, and the walking direction
 * and animation counter update from the requested velocity before
 * delegating the collision work to entity_move().
 */
uint8_t mob_move(Mob* mob, int xa, int ya) {
	if (mob->entity.vt->isSwimming(&mob->entity)) {
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
