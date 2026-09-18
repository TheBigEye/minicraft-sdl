/*
 * grass_tile.c - Behaviour of grass (Java: tile.GrassTile).
 *
 * Shovel -> dirt, hoe -> farmland; both have a 1 in 5 chance of dropping
 * seeds. On top of that, grass now and then spreads onto an adjacent dirt
 * square.
 */
#include "tile.h"
#include "grass_tile.h"

#include <stdlib.h>

#include "../../entity/itementity.h"
#include "../../gfx/color.h"
#include "../../item/item.h"
#include "../../item/resourceitem.h"
#include "../../sound/sound.h"


/* Constructor: grass blends with itself along its edges. */
PUBLIC void grasstile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render   = grasstile_render;
    this->tick     = grasstile_tick;
    this->interact = grasstile_interact;

    this->connects_to_grass = true;
}


/* Shovel -> dirt; hoe -> farmland. Both may drop seeds, 1 in 5. */
PUBLIC boolean grasstile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
    (void) attackDir;

    if (item->id == TOOL) {
        if (item->add.tool.type == SHOVEL) {
            if (player_pay_stamina(player, 4 - item->add.tool.level)) {
                level->set_tile(level, xt, yt, tiles[DIRT], 0);
                sound_play(SND_MONSTERHURT);

                Random* random = &this->random;

                if (random->next_int(random, 5) == 0) {
                    Item drop;
                    ItemEntity* item_entity = new(ItemEntity);

                    resourceitem_create(&drop, &seeds);

                    itementity_create(item_entity, drop,
                            xt * 16 + random->next_int(random, 10) + 3,
                            yt * 16 + random->next_int(random, 10) + 3);
                    level->add(level, &item_entity->entity);
                    return true;
                }
            }
        }

        if (item->add.tool.type == HOE) {
            if (player_pay_stamina(player, 4 - item->add.tool.level)) {
                sound_play(SND_MONSTERHURT);

                Random* random = &this->random;

                if (random->next_int(random, 5) == 0) {
                    Item drop;
                    ItemEntity* item_entity = new(ItemEntity);

                    resourceitem_create(&drop, &seeds);

                    itementity_create(item_entity, drop,
                            xt * 16 + random->next_int(random, 10) + 3,
                            yt * 16 + random->next_int(random, 10) + 3);
                    level->add(level, &item_entity->entity);
                    return true;
                }

                level->set_tile(level, xt, yt, tiles[FARMLAND], 0);
                return true;
            }
        }
    }

    return false;
}


/* Draws the four quadrants, with an edge where the neighbour is not grass. */
PUBLIC void grasstile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    (void) this;

    int col = get_color4(level->grassColor, level->grassColor, level->grassColor + 111, level->grassColor + 111);
    int transitionColor = get_color4(level->grassColor - 111, level->grassColor, level->grassColor + 111, level->dirtColor);

    boolean u = !level->get_tile(level, x, y - 1)->connects_to_grass;
    boolean d = !level->get_tile(level, x, y + 1)->connects_to_grass;
    boolean l = !level->get_tile(level, x - 1, y)->connects_to_grass;
    boolean r = !level->get_tile(level, x + 1, y)->connects_to_grass;

    if (!u && !l) {
        screen->render(screen, x * 16 + 0, y * 16 + 0, 0, col, 0);
    } else {
        screen->render(screen, x * 16 + 0, y * 16 + 0, (l ? 11 : 12) + (u ? 0 : 1) * 32, transitionColor, 0);
    }

    if (!u && !r) {
        screen->render(screen, x * 16 + 8, y * 16 + 0, 1, col, 0);
    } else {
        screen->render(screen, x * 16 + 8, y * 16 + 0, (r ? 13 : 12) + (u ? 0 : 1) * 32, transitionColor, 0);
    }

    if (!d && !l) {
        screen->render(screen, x * 16 + 0, y * 16 + 8, 2, col, 0);
    } else {
        screen->render(screen, x * 16 + 0, y * 16 + 8, (l ? 11 : 12) + (d ? 2 : 1) * 32, transitionColor, 0);
    }

    if (!d && !r) {
        screen->render(screen, x * 16 + 8, y * 16 + 8, 3, col, 0);
    } else {
        screen->render(screen, x * 16 + 8, y * 16 + 8, (r ? 13 : 12) + (d ? 2 : 1) * 32, transitionColor, 0);
    }
}


/* 1 in 40 per tick: spreads the grass onto a random neighbouring dirt. */
PUBLIC void grasstile_tick(Tile* this, Level* level, int xt, int yt) {
    Random* random = &this->random;

    if (random->next_int(random, 40)) return;

    int xn = xt;
    int yn = yt;

    if (random->next_boolean(random)) {
        xn += random->next_int(random, 2) * 2 - 1;
    } else {
        yn += random->next_int(random, 2) * 2 - 1;
    }

    if (level->get_tile(level, xn, yn) == tiles[DIRT]) {
        level->set_tile(level, xn, yn, this, 0);
    }
}
