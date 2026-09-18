/*
 * zombie.c - The Zombie mob (Java: com.mojang.ld22.entity.Zombie).
 *
 * A walking undead enemy; it drops cloth resources on death.
 */
#include "zombie.h"

#include <stdlib.h>

#include "../game.h"
#include "../gfx/color.h"
#include "../gfx/screen.h"
#include "../item/resource/resource.h"
#include "../item/resourceitem.h"
#include "itementity.h"


/* Constructor: spawns a zombie of level `lvl` at a random spot. */
PUBLIC void zombie_create(Zombie* this, int lvl) {
    mob_create(&this->mob);   /* super(): Mob already installs its methods */

    /* What the Zombie class overrides from Mob.
     * Java: class Zombie extends Mob */
    this->mob.entity.tick       = (entity_tick_fn) zombie_tick;
    this->mob.entity.render      = (entity_render_fn) zombie_render;
    this->mob.entity.touched_by  = (entity_touched_by_fn) zombie_touched_by;
    this->mob.entity.die         = (entity_die_fn) zombie_die;

    this->mob.entity.type = ZOMBIE;
    this->mob.entity.x = this->mob.entity.random.next_int(&this->mob.entity.random, 64 * 16);
    this->mob.entity.y = this->mob.entity.random.next_int(&this->mob.entity.random, 64 * 16);
    this->mob.health = this->mob.maxHealth = lvl * lvl * 10;

    this->lvl = lvl;
    this->randomWalkTime = 0;
    this->xa = this->ya = 0;
}


/* Walking AI: it wanders with random direction changes, matching the
 * original's simple undead behaviour. */
PUBLIC void zombie_tick(Zombie* this) {
    mob_tick(&this->mob);   /* Java: super.tick() */

    Random* random = &this->mob.entity.random;

    if (game_player->mob.entity.level == this->mob.entity.level && this->randomWalkTime == 0) {
        int xd = game_player->mob.entity.x - this->mob.entity.x;
        int yd = game_player->mob.entity.y - this->mob.entity.y;

        if (xd * xd + yd * yd < 50 * 50) {
            this->ya = this->xa = 0;

            if (xd < 0) this->xa = -1;
            if (xd > 0) this->xa = 1;
            if (yd < 0) this->ya = -1;
            if (yd > 0) this->ya = 1;
        }
    }

    int speed = this->mob.tickTime & 1;

    if (!this->mob.move(&this->mob, this->xa * speed, this->ya * speed) || random->next_int(random, 200) == 0) {
        this->randomWalkTime = 60;
        this->xa = (random->next_int(random, 3) - 1) * random->next_int(random, 2);
        this->ya = (random->next_int(random, 3) - 1) * random->next_int(random, 2);
    }

    if (this->randomWalkTime > 0) --this->randomWalkTime;
}


/* Draws the zombie sprite with its walk animation and facing. */
PUBLIC void zombie_render(Zombie* this, Screen* screen) {
    int xt = 0;
    int yt = 14;

    int flip1 = (this->mob.walkDist >> 3) & 1;
    int flip2 = (this->mob.walkDist >> 3) & 1;

    if (this->mob.dir == 1) xt += 2;

    if (this->mob.dir > 1) {
        flip1 = 0;
        flip2 = (this->mob.walkDist >> 4) & 1;

        if (this->mob.dir == 2) {
            flip1 = 1;
        }

        xt += 4 + ((this->mob.walkDist >> 3) & 1) * 2;
    }

    int xo = this->mob.entity.x - 8;
    int yo = this->mob.entity.y - 11;

    int col = get_color4(-1, 10, 252, 050);

    if (this->lvl == 2) col = get_color4(-1, 100, 522, 050);
    if (this->lvl == 3) col = get_color4(-1, 111, 444, 050);
    if (this->lvl == 4) col = get_color4(-1, 000, 111, 020);

    if (this->mob.hurtTime > 0) col = get_color4(-1, 555, 555, 555);

    screen->render(screen, xo + 8 * flip1, yo + 0, xt + yt * 32, col, flip1);
    screen->render(screen, xo + 8 - 8 * flip1, yo + 0, xt + 1 + yt * 32, col, flip1);
    screen->render(screen, xo + 8 * flip2, yo + 8, xt + (yt + 1) * 32, col, flip2);
    screen->render(screen, xo + 8 - 8 * flip2, yo + 8, xt + 1 + (yt + 1) * 32, col, flip2);
}


/* Hurts the player on contact, for lvl + 1 damage. */
PUBLIC void zombie_touched_by(Zombie* this, Entity* entity) {
    if (entity->type == PLAYER) {
        entity->hurt(entity, &this->mob, this->lvl + 1, this->mob.dir);
    }
}


/* Drops one or two cloth items around the corpse and awards score. */
PUBLIC void zombie_die(Zombie* this) {
    mob_die(&this->mob);   /* Java: super.die() */

    Random* random = &this->mob.entity.random;

    int count = random->next_int(random, 2) + 1;

    for (int i = 0; i < count; ++i) {
        ItemEntity* item_entity = new(ItemEntity);

        if (!item_entity) {
            continue;
        }

        Item resource;

        resourceitem_create(&resource, &cloth);

        itementity_create(item_entity, resource,
                this->mob.entity.x + random->next_int(random, 11) - 5,
                this->mob.entity.y + random->next_int(random, 11) - 5);

        /* level->add takes ownership of the memory. */
        this->mob.entity.level->add(this->mob.entity.level, &item_entity->entity);
    }

    if (game_player->mob.entity.level == this->mob.entity.level) {
        game_player->score += 50 * this->lvl;
    }
}
