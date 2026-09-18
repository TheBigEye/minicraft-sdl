/*
 * flower_tile.c - Behaviour of the flowers (Java: tile.FlowerTile).
 *
 * They are grass with flowers on top: any tool, or a hit, picks them and
 * leaves grass behind, dropping flowers. The data byte decides which
 * corner the flower is drawn in.
 */
#include "tile.h"
#include "flower_tile.h"

#include <stdlib.h>

#include "../../entity/itementity.h"
#include "../../gfx/color.h"
#include "../../item/item.h"
#include "../../item/resourceitem.h"


/* Constructor: flowers draw on grass, so they share its edges. */
PUBLIC void flowertile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render   = flowertile_render;
    this->hurt     = flowertile_hurt;
    this->interact = flowertile_interact;

    this->connects_to_grass = true;
}


/* Grass underneath, plus one flower on one of the two diagonals. */
PUBLIC void flowertile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    (void) this;

    Tile* grass = tiles[GRASS];

    grass->render(grass, screen, level, x, y);

    int data = level->get_data(level, x, y);
    int shape = (data / 16) % 2;
    int flowerCol = get_color4(10, level->grassColor, 555, 440);

    if (shape == 0) screen->render(screen, x * 16 + 0, y * 16 + 0, 1 + 1 * 32, flowerCol, 0);
    if (shape == 1) screen->render(screen, x * 16 + 8, y * 16 + 0, 1 + 1 * 32, flowerCol, 0);

    if (shape == 1) screen->render(screen, x * 16 + 0, y * 16 + 8, 1 + 1 * 32, flowerCol, 0);
    if (shape == 0) screen->render(screen, x * 16 + 8, y * 16 + 8, 1 + 1 * 32, flowerCol, 0);
}


/* Picking with the shovel: clears the flowers and drops two of them. */
PUBLIC boolean flowertile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
    (void) attackDir;

    if (item->id == TOOL) {
        if (item->add.tool.type == SHOVEL) {
            if (player_pay_stamina(player, 4 - item->add.tool.level)) {
                Random* random = &this->random;

                ItemEntity* entity = new(ItemEntity);
                Item drop;

                resourceitem_create(&drop, &flower);

                itementity_create(entity, drop,
                        xt * 16 + random->next_int(random, 10) + 3,
                        yt * 16 + random->next_int(random, 10) + 3);
                level->add(level, (Entity*) entity);

                entity = new(ItemEntity);
                itementity_create(entity, drop,
                        xt * 16 + random->next_int(random, 10) + 3,
                        yt * 16 + random->next_int(random, 10) + 3);
                level->add(level, (Entity*) entity);

                level->set_tile(level, xt, yt, tiles[GRASS], 0);
                return true;
            }
        }
    }

    return false;
}


/* A hit picks them too, dropping 1-2 flowers. */
PUBLIC void flowertile_hurt(Tile* this, Level* level, int x, int y, Mob* source, int dmg, int attackDir) {
    (void) source;
    (void) dmg;
    (void) attackDir;

    Random* random = &this->random;

    int count = random->next_int(random, 2) + 1;

    for (int i = 0; i < count; i++) {
        ItemEntity* ent = new(ItemEntity);
        Item res;

        resourceitem_create(&res, &flower);

        int xx = x * 16 + random->next_int(random, 10) + 3;
        int yy = y * 16 + random->next_int(random, 10) + 3;

        itementity_create(ent, res, xx, yy);
        level->add(level, (Entity*) ent);
    }

    level->set_tile(level, x, y, tiles[GRASS], 0);
}
