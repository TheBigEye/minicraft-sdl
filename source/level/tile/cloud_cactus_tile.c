/*
 * cloud_cactus_tile.c - Behaviour of the cloud cactus
 *                       (Java: tile.CloudCactusTile).
 *
 * Only the Air Wizard gets through. Touching it deals 3 damage, and the
 * pickaxe chips at it: after 10 hits it becomes cloud again.
 */
#include "tile.h"
#include "cloud_cactus_tile.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../entity/particle/smashparticle.h"
#include "../../entity/particle/textparticle.h"
#include "../../gfx/color.h"
#include "../../item/item.h"


/* Constructor: a sky obstacle, harmful on contact. */
PUBLIC void cloudcactustile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render      = cloudcactustile_render;
    this->may_pass    = cloudcactustile_may_pass;
    this->hurt        = cloudcactustile_hurt;
    this->bumped_into = cloudcactustile_bumped_into;
    this->interact    = cloudcactustile_interact;
}


/* Draws the cactus on the cloud background. */
PUBLIC void cloudcactustile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    (void) this;
    (void) level;

    int col = get_color4(444, 111, 333, 555);

    screen->render(screen, x * 16 + 0, y * 16 + 0, 17 + 1 * 32, col, 0);
    screen->render(screen, x * 16 + 8, y * 16 + 0, 18 + 1 * 32, col, 0);
    screen->render(screen, x * 16 + 0, y * 16 + 8, 17 + 2 * 32, col, 0);
    screen->render(screen, x * 16 + 8, y * 16 + 8, 18 + 2 * 32, col, 0);
}


/* Only the Air Wizard gets through. Java: CloudCactusTile.mayPass() */
PUBLIC boolean cloudcactustile_may_pass(Tile* this, Level* level, int x, int y, Entity* e) {
    (void) this;
    (void) level;
    (void) x;
    (void) y;

#ifdef GODMODE
    if (e->type == PLAYER) return true;
#endif

    return e->type == AIRWIZARD;
}


/* Pricks hard, 3 damage, everyone but the Air Wizard. */
PUBLIC void cloudcactustile_bumped_into(Tile* this, Level* level, int xt, int yt, Entity* entity) {
    (void) level;

    if (entity->type == AIRWIZARD) return;

    entity->hurt_tile(entity, this->id, xt, yt, 3);
}


/*
 * A swing: shows the hit and, only if the damage is real, accumulates up
 * to 10 before turning back into cloud.
 */
PUBLIC void cloudcactustile_hurt_dmg(Tile* this, Level* level, int x, int y, int dmg) {
    (void) this;

    int damage = level->get_data(level, x, y) + 1;

    SmashParticle* smash = new(SmashParticle);

    smashparticle_create(smash, x * 16 + 8, y * 16 + 8);
    level->add(level, &smash->entity);

    TextParticle* text = new(TextParticle);
    String txt = new_array(char, 16);

    sprintf(txt, "%d", dmg);
    textparticle_create(text, txt, x * 16 + 8, y * 16 + 8, get_color4(-1, 500, 500, 500));
    level->add(level, &text->entity);

    if (dmg > 0) {
        if (damage >= 10) {
            level->set_tile(level, x, y, tiles[CLOUD], 0);
        } else {
            level->set_data(level, x, y, damage);
        }
    }
}


/* The pickaxe mines it, spending stamina. */
PUBLIC boolean cloudcactustile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
    (void) attackDir;

    if (item->id == TOOL) {
        if (item->add.tool.type == PICKAXE) {
            if (player_pay_stamina(player, 6 - item->add.tool.level)) {
                cloudcactustile_hurt_dmg(this, level, xt, yt, 1);
                return true;
            }
        }
    }

    return false;
}


/* Being attacked does not break it: it only shows the impact. */
PUBLIC void cloudcactustile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
    (void) source;
    (void) dmg;
    (void) attackDir;

    cloudcactustile_hurt_dmg(this, level, x, y, 0);
}
