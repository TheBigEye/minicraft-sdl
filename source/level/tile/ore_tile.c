/*
 * ore_tile.c - Behaviour of an ore vein (Java: tile.OreTile).
 *
 * A single implementation for all three ores: the resource it drops, and
 * with it the colour of the vein, is decided when it is built. The pickaxe
 * wears it down until it turns into dirt.
 */
#include "tile.h"
#include "ore_tile.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../entity/itementity.h"
#include "../../entity/particle/smashparticle.h"
#include "../../entity/particle/textparticle.h"
#include "../../gfx/color.h"
#include "../../item/item.h"
#include "../../item/resource/resource.h"
#include "../../item/resourceitem.h"


/* Constructor: stores the resource to drop and derives the vein's colour. */
PUBLIC void oretile_init(Tile* this, TileID id, Resource* toDrop) {
    tile_init(this, id);

    this->render      = oretile_render;
    this->may_pass    = oretile_may_pass;
    this->hurt        = oretile_hurt;
    this->bumped_into = oretile_bumped_into;
    this->interact    = oretile_interact;

    this->add.ore.toDrop = toDrop;
    this->add.ore.color = toDrop->color & 0xffff00;
}


/* Ore is solid. Java: OreTile.mayPass() { return false; } */
PUBLIC boolean oretile_may_pass(Tile* this, Level* level, int x, int y, Entity* e) {
    (void) this;
    (void) level;
    (void) x;
    (void) y;

#ifdef GODMODE
    if (e->type == PLAYER) return true;
#else
    (void) e;
#endif

    return false;
}


/* Bumping into a vein deals 3 damage. Java: OreTile.bumpedInto() */
PUBLIC void oretile_bumped_into(Tile* this, Level* level, int xt, int yt, Entity* entity) {
    (void) level;

    entity->hurt_tile(entity, this->id, xt, yt, 3);
}


/* Draws the vein, re-tinted every frame with the resource's colour. */
PUBLIC void oretile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    this->add.ore.color = (this->add.ore.toDrop->color & 0xffffff00) + get_color(level->dirtColor);

    screen->render(screen, x * 16 + 0, y * 16 + 0, 17 + 1 * 32, this->add.ore.color, 0);
    screen->render(screen, x * 16 + 8, y * 16 + 0, 18 + 1 * 32, this->add.ore.color, 0);
    screen->render(screen, x * 16 + 0, y * 16 + 8, 17 + 2 * 32, this->add.ore.color, 0);
    screen->render(screen, x * 16 + 8, y * 16 + 8, 18 + 2 * 32, this->add.ore.color, 0);
}


/* A swing: drops part of the resource and, once exhausted, leaves dirt.
 * Java: OreTile.hurt(Level, int, int, int dmg) */
PUBLIC void oretile_hurt_dmg(Tile* this, Level* level, int x, int y, int dmg) {
    int damage = level->get_data(level, x, y) + 1;

    SmashParticle* smash = new(SmashParticle);

    smashparticle_create(smash, x * 16 + 8, y * 16 + 8);
    level->add(level, &smash->entity);

    TextParticle* txt = new(TextParticle);
    String tx_ = new_array(char, 16);

    sprintf(tx_, "%d", dmg);
    textparticle_create(txt, tx_, x * 16 + 8, y * 16 + 8, get_color4(-1, 500, 500, 500));
    level->add(level, &txt->entity);

    Random* random = &this->random;

    if (dmg > 0) {
        int count = random->next_int(random, 2);

        if (damage >= random->next_int(random, 10) + 3) {
            level->set_tile(level, x, y, tiles[DIRT], 0);
            count += 2;
        } else {
            level->set_data(level, x, y, damage);
        }

        for (int i = 0; i < count; ++i) {
            ItemEntity* entity = new(ItemEntity);
            Item item;

            resourceitem_create(&item, this->add.ore.toDrop);

            itementity_create(entity, item,
                    x * 16 + random->next_int(random, 10) + 3,
                    y * 16 + random->next_int(random, 10) + 3);
            level->add(level, (Entity*) entity);
        }
    }
}


/* The pickaxe mines the vein, spending stamina. */
PUBLIC boolean oretile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
    (void) attackDir;

    if (item->id == TOOL) {
        if (item->add.tool.type == PICKAXE) {
            if (player_pay_stamina(player, 6 - item->add.tool.level)) {
                oretile_hurt_dmg(this, level, xt, yt, 1);
                return true;
            }
        }
    }

    return false;
}


/* Generic hits do not damage it: they only show the impact. */
PUBLIC void oretile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
    (void) source;
    (void) dmg;
    (void) attackDir;

    oretile_hurt_dmg(this, level, x, y, 0);
}
