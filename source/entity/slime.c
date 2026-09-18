/*
 * slime.c - The Slime mob (Java: com.mojang.ld22.entity.Slime).
 *
 * A simple hopping enemy: it jumps in random directions, harms the player
 * on contact and drops slime resources when killed.
 */
#include "slime.h"

#include <stdlib.h>

#include "../game.h"
#include "../gfx/color.h"
#include "../gfx/screen.h"
#include "../item/resource/resource.h"
#include "../item/resourceitem.h"
#include "../log.h"
#include "itementity.h"
#include "mob.h"


/*
 * Constructor: spawns a slime of level `lvl` at a random spot; both health
 * and damage scale with the level, health being lvl * lvl * 5.
 */
PUBLIC void slime_create(Slime* this, int lvl) {
    mob_create(&this->mob);   /* super(): Mob already installs its methods */

    /* What the Slime class overrides from Mob.
     * Java: class Slime extends Mob */
    this->mob.entity.tick       = (entity_tick_fn) slime_tick;
    this->mob.entity.render     = (entity_render_fn) slime_render;
    this->mob.entity.touched_by = (entity_touched_by_fn) slime_touched_by;
    this->mob.entity.die        = (entity_die_fn) slime_die;

    if (lvl <= 0) LOG_WARN("Slime of level %d: expected > 0", lvl);

    this->mob.entity.type = SLIME;
    this->mob.entity.x = this->mob.entity.random.next_int(&this->mob.entity.random, 64 * 16);
    this->mob.entity.y = this->mob.entity.random.next_int(&this->mob.entity.random, 64 * 16);
    this->mob.health = this->mob.maxHealth = lvl * lvl * 5;

    this->lvl = lvl;
    this->jumpTime = 0;
    this->xa = this->ya = 0;
}


/*
 * Hopping AI: it keeps jumping along its current direction and picks a new
 * random one when it is blocked or on a whim, as in Java.
 */
PUBLIC void slime_tick(Slime* this) {
    mob_tick(&this->mob);   /* Java: super.tick() */

    Random* random = &this->mob.entity.random;

    int speed = 1;

    if (!mob_move(&this->mob, this->xa * speed, this->ya * speed) || random->next_int(random, 40) == 0) {
        if (this->jumpTime <= -10) {
            this->xa = random->next_int(random, 3) - 1;
            this->ya = random->next_int(random, 3) - 1;

            if (game_player->mob.entity.level == this->mob.entity.level) {
                int xd = game_player->mob.entity.x - this->mob.entity.x;
                int yd = game_player->mob.entity.y - this->mob.entity.y;

                if (xd * xd + yd * yd < 50 * 50) {
                    if (xd < 0) this->xa = -1;
                    if (xd > 0) this->xa = 1;
                    if (yd < 0) this->ya = -1;
                    if (yd > 0) this->ya = 1;
                }
            }

            if (this->xa != 0 || this->ya != 0) this->jumpTime = 10;
        }
    }

    --this->jumpTime;

    if (this->jumpTime == 0) {
        this->xa = this->ya = 0;
    }
}


/* Drops one or two slime resources around the corpse and awards
 * 25 * lvl score when the player shares the level. */
PUBLIC void slime_die(Slime* this) {
    mob_die(&this->mob);   /* Java: super.die() */

    Random* random = &this->mob.entity.random;

    int count = random->next_int(random, 2) + 1;

    for (int i = 0; i < count; ++i) {
        ItemEntity* item_entity = new(ItemEntity);

        if (!item_entity) {
            continue;
        }

        Item resource;

        resourceitem_create(&resource, &slime);

        itementity_create(item_entity, resource,
                this->mob.entity.x + random->next_int(random, 11) - 5,
                this->mob.entity.y + random->next_int(random, 11) - 5);

        /* level->add takes ownership of the memory. */
        this->mob.entity.level->add(this->mob.entity.level, &item_entity->entity);
    }

    if (game_player->mob.entity.level == this->mob.entity.level) {
        game_player->score += 25 * this->lvl;
    }
}


/* Draws the slime body, squashed while a jump is in progress. */
PUBLIC void slime_render(Slime* this, Screen* screen) {
    int xt = 0;
    int yt = 18;

    int xo = this->mob.entity.x - 8;
    int yo = this->mob.entity.y - 11;

    if (this->jumpTime > 0) {
        xt += 2;
        yo -= 4;
    }

    int col = get_color4(-1, 10, 252, 555);

    if (this->lvl == 2) col = get_color4(-1, 100, 522, 555);
    if (this->lvl == 3) col = get_color4(-1, 111, 444, 555);
    if (this->lvl == 4) col = get_color4(-1, 000, 111, 224);

    if (this->mob.hurtTime > 0) {
        col = get_color4(-1, 555, 555, 555);
    }

    screen->render(screen, xo + 0, yo + 0, xt + yt * 32, col, 0);
    screen->render(screen, xo + 8, yo + 0, xt + 1 + yt * 32, col, 0);
    screen->render(screen, xo + 0, yo + 8, xt + (yt + 1) * 32, col, 0);
    screen->render(screen, xo + 8, yo + 8, xt + 1 + (yt + 1) * 32, col, 0);
}


/* Hurts the player on contact, for lvl damage. */
PUBLIC void slime_touched_by(Slime* this, Entity* entity) {
    if (entity->type == PLAYER) {
        entity->hurt(entity, &this->mob, this->lvl, this->mob.dir);
    }
}
