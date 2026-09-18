/*
 * cactus_tile.c - Behaviour of the cactus (Java: tile.CactusTile).
 *
 * It accumulates damage in the data byte, healing it as it goes: at 10
 * points it breaks, leaves sand behind and drops 1-2 cactus flowers. It
 * also hurts whoever bumps into it.
 */
#include "tile.h"
#include "cactus_tile.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../entity/itementity.h"
#include "../../entity/particle/smashparticle.h"
#include "../../entity/particle/textparticle.h"
#include "../../gfx/color.h"
#include "../../item/resourceitem.h"


/* Constructor: a solid cactus that blends with neighbouring sand. */
PUBLIC void cactustile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render      = cactustile_render;
    this->may_pass    = cactustile_may_pass;
    this->hurt        = cactustile_hurt;
    this->bumped_into = cactustile_bumped_into;
    this->tick        = cactustile_tick;

    this->connects_to_sand = true;
}


/* Draws the cactus on top of the sand background. */
PUBLIC void cactustile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    (void) this;

    int col = get_color4(20, 40, 50, level->sandColor);

    screen->render(screen, x * 16 + 0, y * 16 + 0, 8 + 2 * 32, col, 0);
    screen->render(screen, x * 16 + 8, y * 16 + 0, 9 + 2 * 32, col, 0);
    screen->render(screen, x * 16 + 0, y * 16 + 8, 8 + 3 * 32, col, 0);
    screen->render(screen, x * 16 + 8, y * 16 + 8, 9 + 3 * 32, col, 0);
}


/* The cactus is solid: it cannot be crossed. */
PUBLIC boolean cactustile_may_pass(Tile* this, Level* level, int x, int y, Entity* e) {
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


/* Pricks whoever bumps into it: 1 damage.
 * Java: entity.hurt(this, x, y, 1), the hurt(Tile, int, int, int) overload. */
PUBLIC void cactustile_bumped_into(Tile* this, Level* level, int xt, int yt, Entity* entity) {
    (void) level;

    entity->hurt_tile(entity, this->id, xt, yt, 1);
}


/* Damage adds up: at 10 points it breaks, dropping cactus flowers. */
PUBLIC void cactustile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
    (void) source;
    (void) attackDir;

    int damage = level->get_data(level, x, y) + dmg;
    Random* random = &this->random;

    SmashParticle* smash = new(SmashParticle);

    smashparticle_create(smash, x * 16 + 8, y * 16 + 8);
    level->add(level, &smash->entity);

    TextParticle* text = new(TextParticle);
    String txt = new_array(char, 16);

    sprintf(txt, "%d", dmg);
    textparticle_create(text, txt, x * 16 + 8, y * 16 + 8, get_color4(-1, 500, 500, 500));
    level->add(level, &text->entity);

    if (damage >= 10) {
        int count = random->next_int(random, 2) + 1;

        for (int i = 0; i < count; ++i) {
            Item res;
            ItemEntity* ent = new(ItemEntity);

            resourceitem_create(&res, &cactusFlower);

            int xx = x * 16 + random->next_int(random, 10) + 3;
            int yy = y * 16 + random->next_int(random, 10) + 3;

            itementity_create(ent, res, xx, yy);
            level->add(level, (Entity*) ent);
        }

        level->set_tile(level, x, y, tiles[SAND], 0);

    } else {
        level->set_data(level, x, y, damage);
    }
}


/*
 * Heals one point of accumulated damage per tick, if any is left.
 * Java: `if (damage > 0)`; testing only for non-zero would keep pushing a
 * negative data byte further down instead of leaving it alone.
 */
PUBLIC void cactustile_tick(Tile* this, Level* level, int xt, int yt) {
    (void) this;

    int damage = level->get_data(level, xt, yt);

    if (damage > 0) {
        level->set_data(level, xt, yt, damage - 1);
    }
}
