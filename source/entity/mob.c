/*
 * mob.c - The behaviour shared by every living entity
 *         (Java: the body of the Mob class).
 *
 * Health bookkeeping, knockback, swimming, spawn placement and the
 * hurt/die flow live here; the concrete mobs, that is, Player, Slime,
 * Zombie and AirWizard, override only what their Java classes override.
 */
#include "mob.h"

#include <stdio.h>
#include <stdlib.h>

#include "../game.h"
#include "../gfx/color.h"
#include "../level/tile/tile.h"
#include "../level/tile/tileids.h"
#include "../sound/sound.h"
#include "entity.h"
#include "particle/textparticle.h"


/*
 * Constructor (Java: Mob()). It starts from the base class, entity_create
 * acting as `super()`, and then overrides the Entity methods Mob
 * redefines, plus its own three. Subclasses call this first and only
 * replace what they in turn override.
 */
PUBLIC void mob_create(Mob* this) {
    entity_create(&this->entity);   /* super() */

    /* What Mob overrides from Entity. */
    this->entity.tick        = (entity_tick_fn) mob_tick;
    this->entity.blocks      = (entity_blocks_fn) mob_blocks;
    this->entity.hurt        = (entity_hurt_fn) mob_hurt;
    this->entity.hurt_tile   = (entity_hurt_tile_fn) mob_hurt_tile;
    this->entity.die         = (entity_die_fn) mob_die;
    this->entity.do_hurt     = (entity_do_hurt_fn) mob_do_hurt;
    this->entity.is_swimming = (entity_is_swimming_fn) mob_is_swimming;

    /* Mob's own methods. */
    this->move           = mob_move;
    this->find_start_pos = mob_find_start_pos;
    this->heal           = mob_heal;

    this->walkDist = this->dir = this->hurtTime = 0;
    this->xKnockback = this->yKnockback = 0;
    this->maxHealth = 10;
    this->health = this->maxHealth;
    this->swimTimer = this->tickTime = 0;

    this->entity.x = this->entity.y = 8;
    this->entity.xr = 4;
    this->entity.yr = 3;
}


/* Class-tag test for the four Mob subclasses (Java: instanceof Mob). */
PUBLIC boolean entity_is_mob(Entity* entity) {
    switch (entity->type) {
        case SLIME:
        case ZOMBIE:
        case AIRWIZARD:
        case PLAYER:
            return true;
        default:
            return false;
    }
}


/*
 * Applies damage to the mob (Java: Mob.doHurt). While the hurt cooldown
 * runs the mob is invulnerable. It plays monsterHurt when the player is
 * within 80 pixels, spawns a red damage number, subtracts health and
 * queues knockback opposite to the attack direction.
 */
PUBLIC void mob_do_hurt(Mob* this, int damage, int attackDir) {
    if (this->hurtTime > 0) return;

    /* Java: `if (level.player != null)`. Here the mob is already in a
     * level, so `this->entity.level` is the level to test. */
    if (game_player && game_player->mob.entity.level == this->entity.level) {
        /* Sound.monsterHurt.play(), only if the player is close by.
         * Java: Mob.doHurt, the distance check against level.player. */
        int xd = game_player->mob.entity.x - this->entity.x;
        int yd = game_player->mob.entity.y - this->entity.y;

        if (xd * xd + yd * yd < 80 * 80) {
            sound_play(SND_MONSTERHURT);
        }
    }

    TextParticle* text_particle = new(TextParticle);
    String text = new_array(char, 16);

    sprintf(text, "%d", damage);
    textparticle_create(text_particle, text, this->entity.x, this->entity.y, get_color4(-1, 500, 500, 500));
    this->entity.level->add(this->entity.level, &text_particle->entity);

    this->health -= damage;

    if (attackDir == 0) this->yKnockback = 6;
    if (attackDir == 1) this->yKnockback = -6;
    if (attackDir == 2) this->xKnockback = -6;
    if (attackDir == 3) this->xKnockback = 6;

    this->hurtTime = 10;
}


/* Entry point for mob-on-mob damage: it dispatches to the virtual
 * do_hurt, so Player's override, with its invulnerability frames and
 * death, applies.
 * Java: Mob.hurt(Mob mob, int damage, int attackDir) */
PUBLIC void mob_hurt(Mob* this, Mob* by, int damage, int attackDir) {
    (void) by;

    /* Java: doHurt() is virtual, Player overrides it. */
    this->entity.do_hurt(&this->entity, damage, attackDir);
}


/*
 * Damage dealt by the tile the mob stands on, lava or a cactus; the
 * attack direction is the opposite of the mob's facing, as in Java.
 * Java: Mob.hurt(Tile tile, int x, int y, int damage)
 */
PUBLIC void mob_hurt_tile(Mob* this, TileID tile, int x, int y, int damage) {
    (void) tile;
    (void) x;
    (void) y;

    int attackDir = this->dir ^ 1;

    this->entity.do_hurt(&this->entity, damage, attackDir);
}


/* Collision query: whether `entity` stops this mob's movement. The
 * decision is delegated to the other entity, mirroring Java. */
PUBLIC boolean mob_blocks(Mob* this, Entity* entity) {
    /* Java: Mob.blocks(e) { return e.isBlockableBy(this); } */
    return entity->is_blockable_by(entity, this);
}


/*
 * Tries one random spawn position (Java: Mob.findStartPos): it rejects
 * spots within 80 pixels of the player and spots whose neighbourhood,
 * monsterDensity tiles around, already holds entities, then requires the
 * tile itself to let this mob pass. Returns success; whoever spawns
 * retries until it succeeds.
 */
PUBLIC boolean mob_find_start_pos(Mob* this, Level* level) {
    Random* random = &this->entity.random;

    int x = random->next_int(random, level->w);
    int y = random->next_int(random, level->h);

    int xx = x * 16 + 8;
    int yy = y * 16 + 8;

    /*
     * Java: `if (level.player != null)`; a Level carries its own player
     * reference, which is null for every level the player is not on. This
     * port has no such field, so "the player is standing on this level"
     * is spelled `game_player->mob.entity.level == level`.
     *
     * Note it must be compared against `level`, the argument, and not
     * against `this->entity.level`: find_start_pos() runs *before*
     * level_add(), so the candidate mob has no level yet and its field is
     * still null. Comparing it against the mob's own (null) level made
     * the test always false, silently disabling this guard and allowing
     * mobs to spawn right on top of the player.
     */
    if (game_player && game_player->mob.entity.level == level) {
        int xd = game_player->mob.entity.x - xx;
        int yd = game_player->mob.entity.y - yy;

        if (xd * xd + yd * yd < 80 * 80) {
            return false;
        }
    }

    int r = level->monsterDensity * 16;

    ArrayList ents;

    arraylist_create(&ents);

    level->get_entities(level, &ents, xx - r, yy - r, xx + r, yy + r);

    int sz = ents.size;

    ents.free(&ents);

    if (sz > 0) return false;

    Tile* tile = level->get_tile(level, x, y);

    if (tile->may_pass(tile, level, x, y, &this->entity)) {
        this->entity.x = xx;
        this->entity.y = yy;
        return true;
    }

    return false;
}


/* Base death: it only removes the entity; Player and AirWizard override
 * it to drop loot, play sounds or end the game. */
PUBLIC void mob_die(Mob* this) {
    entity_remove(&this->entity);
}


/*
 * Per-tick update shared by all mobs (Java: Mob.tick): lava under the
 * mob hurts it, zero or less health triggers the virtual die(), and the
 * hurt invulnerability cooldown decays.
 */
PUBLIC void mob_tick(Mob* this) {
    ++this->tickTime;

    Tile* under = this->entity.level->get_tile(this->entity.level, this->entity.x >> 4, this->entity.y >> 4);

    if (under == tiles[LAVA]) {
        mob_hurt(this, this, 4, this->dir ^ 1);
    }

    if (this->health <= 0) {
        /* Java: die() is virtual. */
        this->entity.die(&this->entity);
    }

    if (this->hurtTime > 0) {
        --this->hurtTime;
    }
}


/* Swimming test: the tile under the mob's centre is water or lava. */
PUBLIC boolean mob_is_swimming(Mob* this) {
    Tile* tile = this->entity.level->get_tile(this->entity.level, this->entity.x >> 4, this->entity.y >> 4);

    return tile == tiles[WATER] || tile == tiles[LAVA];
}


/* Heals the mob, though not while recently hurt, shows a green number
 * popup and clamps health at maxHealth, as Java's Mob.heal(). */
PUBLIC void mob_heal(Mob* this, int heal) {
    if (this->hurtTime > 0) {
        return;
    }

    String text = new_array(char, 16);

    sprintf(text, "%d", heal);

    TextParticle* text_particle = new(TextParticle);

    textparticle_create(text_particle, text, this->entity.x, this->entity.y, get_color4(-1, 50, 50, 50));
    this->entity.level->add(this->entity.level, &text_particle->entity);

    this->health += heal;

    if (this->health > this->maxHealth) {
        this->health = this->maxHealth;
    }
}


/*
 * Mob movement (Java: Mob.move): swimming halves the effective speed,
 * since every other tick is skipped, pending knockback is applied one
 * pixel at a time, hurt mobs cannot walk, and the walking direction and
 * the animation counter update from the requested velocity before the
 * collision work is delegated to entity_move().
 */
PUBLIC boolean mob_move(Mob* this, int xa, int ya) {
    if (this->entity.is_swimming(&this->entity)) {
        if (this->swimTimer++ % 2 == 0) {
            return true;
        }
    }

    if (this->xKnockback < 0) {
        entity_move2(&this->entity, -1, 0);
        ++this->xKnockback;
    }

    if (this->xKnockback > 0) {
        entity_move2(&this->entity, 1, 0);
        --this->xKnockback;
    }

    if (this->yKnockback < 0) {
        entity_move2(&this->entity, 0, -1);
        ++this->yKnockback;
    }

    if (this->yKnockback > 0) {
        entity_move2(&this->entity, 0, 1);
        --this->yKnockback;
    }

    if (this->hurtTime > 0) return true;

    if (xa != 0 || ya != 0) {
        ++this->walkDist;

        if (xa < 0) this->dir = 2;
        if (xa > 0) this->dir = 3;
        if (ya < 0) this->dir = 1;
        if (ya > 0) this->dir = 0;
    }

    return entity_move(&this->entity, xa, ya);
}
