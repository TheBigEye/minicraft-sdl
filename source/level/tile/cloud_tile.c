/*
 * cloud_tile.c - Behaviour of the cloud (Java: tile.CloudTile).
 *
 * Walkable floor of the sky. The shovel digs through it, opening an
 * infinite fall, and drops cloud.
 */
#include "tile.h"
#include "cloud_tile.h"

#include <stdlib.h>

#include "../../entity/itementity.h"
#include "../../gfx/color.h"
#include "../../item/item.h"
#include "../../item/resource/resource.h"
#include "../../item/resourceitem.h"


/* Constructor: cloud is walkable and is dug with the shovel. */
PUBLIC void cloudtile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render   = cloudtile_render;
    this->interact = cloudtile_interact;
}


/* Draws the cloud floor, with an edge where the infinite fall begins. */
PUBLIC void cloudtile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    (void) this;

    int col = get_color4(444, 444, 555, 555);
    int transitionColor = get_color4(333, 444, 555, -1);

    boolean u = level->get_tile(level, x, y - 1) == tiles[INFINITE_FALL];
    boolean d = level->get_tile(level, x, y + 1) == tiles[INFINITE_FALL];
    boolean l = level->get_tile(level, x - 1, y) == tiles[INFINITE_FALL];
    boolean r = level->get_tile(level, x + 1, y) == tiles[INFINITE_FALL];

    boolean ul = level->get_tile(level, x - 1, y - 1) == tiles[INFINITE_FALL];
    boolean dl = level->get_tile(level, x - 1, y + 1) == tiles[INFINITE_FALL];
    boolean ur = level->get_tile(level, x + 1, y - 1) == tiles[INFINITE_FALL];
    boolean dr = level->get_tile(level, x + 1, y + 1) == tiles[INFINITE_FALL];

    if (!u && !l) {
        if (!ul) screen->render(screen, x * 16 + 0, y * 16 + 0, 17, col, 0);
        else screen->render(screen, x * 16 + 0, y * 16 + 0, 7 + 0 * 32, transitionColor, 3);
    } else {
        screen->render(screen, x * 16 + 0, y * 16 + 0, (l ? 6 : 5) + (u ? 2 : 1) * 32, transitionColor, 3);
    }

    if (!u && !r) {
        if (!ur) screen->render(screen, x * 16 + 8, y * 16 + 0, 18, col, 0);
        else screen->render(screen, x * 16 + 8, y * 16 + 0, 8 + 0 * 32, transitionColor, 3);
    } else {
        screen->render(screen, x * 16 + 8, y * 16 + 0, (r ? 4 : 5) + (u ? 2 : 1) * 32, transitionColor, 3);
    }

    if (!d && !l) {
        if (!dl) screen->render(screen, x * 16 + 0, y * 16 + 8, 20, col, 0);
        else screen->render(screen, x * 16 + 0, y * 16 + 8, 7 + 1 * 32, transitionColor, 3);
    } else {
        screen->render(screen, x * 16 + 0, y * 16 + 8, (l ? 6 : 5) + (d ? 0 : 1) * 32, transitionColor, 3);
    }

    if (!d && !r) {
        if (!dr) screen->render(screen, x * 16 + 8, y * 16 + 8, 19, col, 0);
        else screen->render(screen, x * 16 + 8, y * 16 + 8, 8 + 1 * 32, transitionColor, 3);
    } else {
        screen->render(screen, x * 16 + 8, y * 16 + 8, (r ? 4 : 5) + (d ? 0 : 1) * 32, transitionColor, 3);
    }
}


/* Digging with the shovel opens the fall and drops 1-2 clouds. */
PUBLIC boolean cloudtile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
    (void) attackDir;

    if (item->id == TOOL) {
        if (item->add.tool.type == SHOVEL) {
            if (player_pay_stamina(player, 5)) {
                Random* random = &this->random;

                int count = random->next_int(random, 2) + 1;

                for (int i = 0; i < count; ++i) {
                    ItemEntity* entity = new(ItemEntity);
                    Item drop;

                    resourceitem_create(&drop, &cloud);

                    itementity_create(entity, drop,
                            xt * 16 + random->next_int(random, 10) + 3,
                            yt * 16 + random->next_int(random, 10) + 3);
                    level->add(level, (Entity*) entity);
                }

                return true;
            }
        }
    }

    return false;
}
