/*
 * dirt_tile.c - Behaviour of dirt (Java: tile.DirtTile).
 *
 * The shovel digs a hole and drops dirt, the hoe turns the square into
 * farmland. Both actions cost stamina.
 */
#include "tile.h"
#include "dirt_tile.h"

#include <stdlib.h>

#include "../../entity/itementity.h"
#include "../../gfx/color.h"
#include "../../item/resourceitem.h"
#include "../../sound/sound.h"


/* Constructor: dirt is walkable and worked with a shovel or a hoe. */
PUBLIC void dirttile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render   = dirttile_render;
    this->interact = dirttile_interact;
}


/* Shovel -> hole + loose dirt; hoe -> farmland. */
PUBLIC boolean dirttile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
    (void) attackDir;

    if (item->id == TOOL) {
        if (item->add.tool.type == SHOVEL) {
            if (player_pay_stamina(player, 4 - item->add.tool.level)) {
                level->set_tile(level, xt, yt, tiles[HOLE], 0);

                Random* random = &this->random;
                ItemEntity* entity = new(ItemEntity);
                Item drop;

                resourceitem_create(&drop, &dirt);

                itementity_create(entity, drop,
                        xt * 16 + random->next_int(random, 10) + 3,
                        yt * 16 + random->next_int(random, 10) + 3);
                level->add(level, (Entity*) entity);

                sound_play(SND_MONSTERHURT);
                return true;
            }
        }

        if (item->add.tool.type == HOE) {
            if (player_pay_stamina(player, 4 - item->add.tool.level)) {
                level->set_tile(level, xt, yt, tiles[FARMLAND], 0);
                sound_play(SND_MONSTERHURT);
                return true;
            }
        }
    }

    return false;
}


/* Draws the four quadrants of dirt with the level's palette. */
PUBLIC void dirttile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    (void) this;

    int col = get_color4(level->dirtColor, level->dirtColor, level->dirtColor - 111, level->dirtColor - 111);

    screen->render(screen, x * 16 + 0, y * 16 + 0, 0, col, 0);
    screen->render(screen, x * 16 + 8, y * 16 + 0, 1, col, 0);
    screen->render(screen, x * 16 + 0, y * 16 + 8, 2, col, 0);
    screen->render(screen, x * 16 + 8, y * 16 + 8, 3, col, 0);
}
