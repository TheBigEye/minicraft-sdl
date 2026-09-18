/*
 * furniture_item.c - The furniture item
 *                    (Java: com.mojang.ld22.item.FurnitureItem).
 */
#include "furniture_item.h"

#include <stdlib.h>
#include <string.h>

#include "../entity/furniture.h"
#include "../gfx/color.h"
#include "../gfx/font.h"
#include "../gfx/screen.h"
#include "item.h"


/* Constructor: tags the item as furniture and takes ownership of the
 * entity. Java: FurnitureItem(Furniture furniture) */
PUBLIC void furnitureitem_create(Item* this, struct Furniture* furniture) {
    this->id = FURNITURE;
    this->add.furniture.furniture = furniture;
    this->add.furniture.placed = false;
}


/* Java: FurnitureItem.getColor(). The icon color comes from the carried
 * furniture. */
PUBLIC int furnitureitem_get_color(Item* this) {
    return this->add.furniture.furniture->col;
}


/* Java: FurnitureItem.getSprite(). The icon sprite is the furniture sprite
 * offset into the item row. */
PUBLIC int furnitureitem_get_sprite(Item* this) {
    return this->add.furniture.furniture->sprite + 10 * 32;
}


/* Java: FurnitureItem.renderIcon(Screen, int, int) */
PUBLIC void furnitureitem_render_icon(Item* this, Screen* screen, int x, int y) {
    screen->render(screen, x, y, furnitureitem_get_sprite(this), furnitureitem_get_color(this), 0);
}


/* Java: FurnitureItem.renderInventory(Screen, int, int). Draws the icon
 * plus the furniture's own name in the inventory row. */
PUBLIC void furnitureitem_render_inventory(Item* this, Screen* screen, int x, int y) {
    screen->render(screen, x, y, furnitureitem_get_sprite(this), furnitureitem_get_color(this), 0);
    font_draw(this->add.furniture.furniture->name, strlen(this->add.furniture.furniture->name), screen, x + 8, y, get_color4(-1, 555, 555, 555));
}


/*
 * Java: FurnitureItem.interactOn(Tile, Level, int, int, Player, int).
 *
 * Drops the carried furniture centred on the tile when that tile allows it;
 * the item is then marked placed, because the level owns the entity from
 * that point on.
 */
PUBLIC boolean furnitureitem_interact_on(Item* this, TileID tile, Level* level, int xt, int yt, struct Player* player, int attackDir) {
    (void) player;
    (void) attackDir;

    if (tiles[tile]->may_pass(tiles[tile], level, xt, yt, (Entity *) this->add.furniture.furniture)) {
        this->add.furniture.furniture->entity.x = xt * 16 + 8;
        this->add.furniture.furniture->entity.y = yt * 16 + 8;

        level->add(level, (Entity *) this->add.furniture.furniture);

        this->add.furniture.placed = true;
        this->add.furniture.furniture = null;

        return true;
    }

    return false;
}
