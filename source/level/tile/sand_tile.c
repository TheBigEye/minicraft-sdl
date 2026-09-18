/*
 * sand_tile.c - Behaviour of sand (Java: tile.SandTile).
 *
 * The shovel turns it into dirt and drops a sand resource. When a mob
 * steps on it the data byte is marked and the tile draws footprints, which
 * fade away little by little.
 */
#include "tile.h"
#include "sand_tile.h"

#include <stdlib.h>

#include "../../entity/itementity.h"
#include "../../gfx/color.h"
#include "../../item/item.h"
#include "../../item/resourceitem.h"


/* Constructor: sand blends with its neighbouring sand. */
PUBLIC void sandtile_init(Tile* this, TileID id) {
    tile_init(this, id);

    this->render     = sandtile_render;
    this->tick       = sandtile_tick;
    this->stepped_on = sandtile_stepped_on;
    this->interact   = sandtile_interact;

    this->connects_to_sand = true;
}


/* The shovel digs: it becomes dirt and drops sand. */
PUBLIC boolean sandtile_interact(Tile* this, Level* level, int xt, int yt, Player* player, Item* item, int attackDir) {
    (void) attackDir;

    if (item->id == TOOL) {
        if (item->add.tool.type == SHOVEL) {
            if (player_pay_stamina(player, 4 - item->add.tool.level)) {
                level->set_tile(level, xt, yt, tiles[DIRT], 0);

                Random* random = &this->random;

                ItemEntity* entity = new(ItemEntity);
                Item drop;

                resourceitem_create(&drop, &sand);

                itementity_create(entity, drop,
                        xt * 16 + random->next_int(random, 10) + 3,
                        yt * 16 + random->next_int(random, 10) + 3);
                level->add(level, (Entity*) entity);
                return true;
            }
        }
    }

    return false;
}


/* A mob that steps on it leaves a footprint; tick() consumes the data. */
PUBLIC void sandtile_stepped_on(Tile* this, Level* level, int xt, int yt, Entity* entity) {
    (void) this;

    if (entity_is_mob(entity)) {
        level->set_data(level, xt, yt, 10);
    }
}


/*
 * Footprints fade one step per tick.
 * Java: `if (d > 0)`; testing only for non-zero would keep pushing a
 * negative data byte further down instead of leaving it alone.
 */
PUBLIC void sandtile_tick(Tile* this, Level* level, int xt, int yt) {
    (void) this;

    int damage = level->get_data(level, xt, yt);

    if (damage > 0) level->set_data(level, xt, yt, damage - 1);
}


/* Draws the four quadrants, with a footprint sprite where it was stepped on. */
PUBLIC void sandtile_render(Tile* this, Screen* screen, Level* level, int x, int y) {
    (void) this;

    int col = get_color4(level->sandColor + 2, level->sandColor, level->sandColor - 110, level->sandColor - 110);
    int transitionColor = get_color4(level->sandColor - 110, level->sandColor, level->sandColor - 110, level->dirtColor);

    boolean u = !level->get_tile(level, x, y - 1)->connects_to_sand;
    boolean d = !level->get_tile(level, x, y + 1)->connects_to_sand;
    boolean l = !level->get_tile(level, x - 1, y)->connects_to_sand;
    boolean r = !level->get_tile(level, x + 1, y)->connects_to_sand;
    boolean steppedOn = level->get_data(level, x, y) > 0;

    if (!u && !l) {
        if (!steppedOn) screen->render(screen, x * 16 + 0, y * 16 + 0, 0, col, 0);
        else screen->render(screen, x * 16 + 0, y * 16 + 0, 3 + 1 * 32, col, 0);
    } else {
        screen->render(screen, x * 16 + 0, y * 16 + 0, (l ? 11 : 12) + (u ? 0 : 1) * 32, transitionColor, 0);
    }

    if (!u && !r) screen->render(screen, x * 16 + 8, y * 16 + 0, 1, col, 0);
    else screen->render(screen, x * 16 + 8, y * 16 + 0, (r ? 13 : 12) + (u ? 0 : 1) * 32, transitionColor, 0);

    if (!d && !l) screen->render(screen, x * 16 + 0, y * 16 + 8, 2, col, 0);
    else screen->render(screen, x * 16 + 0, y * 16 + 8, (l ? 11 : 12) + (d ? 2 : 1) * 32, transitionColor, 0);

    if (!d && !r) {
        if (!steppedOn) screen->render(screen, x * 16 + 8, y * 16 + 8, 3, col, 0);
        else screen->render(screen, x * 16 + 8, y * 16 + 8, 3 + 1 * 32, col, 0);
    } else {
        screen->render(screen, x * 16 + 8, y * 16 + 8, (r ? 13 : 12) + (d ? 2 : 1) * 32, transitionColor, 0);
    }
}
