/*
 * tree_tile.c - Behaviour of the tree (Java: tile.TreeTile).
 *
 * Damage accumulates in the data byte: at 20 points the tree falls,
 * leaving grass, and drops wood and acorns. Every hit may also shake an
 * apple loose.
 */
#include "tile.h"
#include "tree_tile.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../entity/itementity.h"
#include "../../entity/particle/smashparticle.h"
#include "../../entity/particle/textparticle.h"
#include "../../gfx/color.h"
#include "../../item/resourceitem.h"


/* Constructor: a solid tree, blending with grass and felled with an axe. */
PUBLIC void treetile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render   = treetile_render;
    this->may_pass = treetile_may_pass;
    this->hurt     = treetile_hurt;
    this->interact = treetile_interact;
    this->tick     = treetile_tick;

    this->connects_to_grass = true;
}


/* The tree is solid. Java: TreeTile.mayPass() { return false; } */
PUBLIC boolean treetile_may_pass(Tile* this, Level* level, int x, int y, Entity* e) {
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


/*
 * Accumulated damage: it may shake an apple loose, shows the hit and, at
 * 20, fells the tree, dropping wood and acorns.
 * Java: TreeTile.hurt(Level, int, int, int dmg)
 */
PUBLIC void treetile_hurt_dmg(Tile* this, Level* level, int x, int y, int dmg) {
    Random* random = &this->random;

    int count = random->next_int(random, 10) == 0 ? 1 : 0;

    for (int i = 0; i < count; i++) {
        Item res;
        ItemEntity* ent = new(ItemEntity);

        resourceitem_create(&res, &apple);

        int xx = x * 16 + random->next_int(random, 10) + 3;
        int yy = y * 16 + random->next_int(random, 10) + 3;

        itementity_create(ent, res, xx, yy);
        level->add(level, &ent->entity);
    }

    int damage = level->get_data(level, x, y) + dmg;

    SmashParticle* smash = new(SmashParticle);

    smashparticle_create(smash, x * 16 + 8, y * 16 + 8);
    level->add(level, &smash->entity);

    TextParticle* text = new(TextParticle);
    String txt = new_array(char, 16);

    sprintf(txt, "%d", dmg);
    textparticle_create(text, txt, x * 16 + 8, y * 16 + 8, get_color4(-1, 500, 500, 500));
    level->add(level, &text->entity);

    if (damage >= 20) {
        count = random->next_int(random, 2) + 1;

        for (int i = 0; i < count; ++i) {
            Item res;
            ItemEntity* ent = new(ItemEntity);

            resourceitem_create(&res, &wood);

            int xx = x * 16 + random->next_int(random, 10) + 3;
            int yy = y * 16 + random->next_int(random, 10) + 3;

            itementity_create(ent, res, xx, yy);
            level->add(level, &ent->entity);
        }

        count = random->next_int(random, random->next_int(random, 4) + 1);

        for (int i = 0; i < count; ++i) {
            Item res;
            ItemEntity* ent = new(ItemEntity);

            resourceitem_create(&res, &acorn);

            int xx = x * 16 + random->next_int(random, 10) + 3;
            int yy = y * 16 + random->next_int(random, 10) + 3;

            itementity_create(ent, res, xx, yy);
            level->add(level, &ent->entity);
        }

        level->set_tile(level, x, y, tiles[GRASS], 0);

    } else {
        level->set_data(level, x, y, damage);
    }
}


/* A mob's damage comes in through the same path as the axe's. */
PUBLIC void treetile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
    (void) source;
    (void) attackDir;

    treetile_hurt_dmg(this, level, x, y, dmg);
}


/* Draws the canopy, with bark where the neighbours form the trunk. */
PUBLIC void treetile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    int col = get_color4(10, 30, 151, level->grassColor);
    int barkCol1 = get_color4(10, 30, 430, level->grassColor);
    int barkCol2 = get_color4(10, 30, 320, level->grassColor);

    boolean u = level->get_tile(level, x, y - 1) == this;
    boolean l = level->get_tile(level, x - 1, y) == this;
    boolean r = level->get_tile(level, x + 1, y) == this;
    boolean d = level->get_tile(level, x, y + 1) == this;

    boolean ul = level->get_tile(level, x - 1, y - 1) == this;
    boolean ur = level->get_tile(level, x + 1, y - 1) == this;
    boolean dl = level->get_tile(level, x - 1, y + 1) == this;
    boolean dr = level->get_tile(level, x + 1, y + 1) == this;

    if (u && ul && l) {
        screen->render(screen, x * 16 + 0, y * 16 + 0, 10 + 1 * 32, col, 0);
    } else {
        screen->render(screen, x * 16 + 0, y * 16 + 0, 9 + 0 * 32, col, 0);
    }

    if (u && ur && r) {
        screen->render(screen, x * 16 + 8, y * 16 + 0, 10 + 2 * 32, barkCol2, 0);
    } else {
        screen->render(screen, x * 16 + 8, y * 16 + 0, 10 + 0 * 32, col, 0);
    }

    if (d && dl && l) {
        screen->render(screen, x * 16 + 0, y * 16 + 8, 10 + 2 * 32, barkCol2, 0);
    } else {
        screen->render(screen, x * 16 + 0, y * 16 + 8, 9 + 1 * 32, barkCol1, 0);
    }

    if (d && dr && r) {
        screen->render(screen, x * 16 + 8, y * 16 + 8, 10 + 1 * 32, col, 0);
    } else {
        screen->render(screen, x * 16 + 8, y * 16 + 8, 10 + 3 * 32, barkCol2, 0);
    }
}


/* Heals one point of accumulated damage per tick, if any is left. */
PUBLIC void treetile_tick(Tile* this, Level* level, int xt, int yt) {
    (void) this;

    int damage = level->get_data(level, xt, yt);

    /*
     * Java: `if (damage > 0)`. Not `if (damage)`: the data byte is signed
     * in both versions, so a negative value (a tile whose data was set
     * past 127) would otherwise keep decreading here, wrapping around
     * instead of staying where it is.
     */
    if (damage > 0) {
        level->set_data(level, xt, yt, damage - 1);
    }
}


/* The axe fells it: damage proportional to the tool's level. */
PUBLIC boolean treetile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
    (void) attackDir;

    if (item->id == TOOL) {
        if (item->add.tool.type == AXE) {
            if (player_pay_stamina(player, 4 - item->add.tool.level)) {
                int dmg = this->random.next_int(&this->random, 10) + item->add.tool.level * 5 + 10;

                treetile_hurt_dmg(this, level, xt, yt, dmg);
                return true;
            }
        }
    }

    return false;
}
