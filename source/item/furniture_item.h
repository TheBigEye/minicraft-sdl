/*
 * furniture_item.h - The furniture item
 *                    (Java: com.mojang.ld22.item.FurnitureItem): carries a
 *                    furniture entity in the inventory and places it.
 */
#ifndef FURNITURE_ITEM_H
#define FURNITURE_ITEM_H 1

#include "../utils/javalang.h"
#include "item.h"

/* Constructor: wraps an owned furniture entity as an inventory item.
 * Java: FurnitureItem(Furniture furniture) */
PUBLIC void furnitureitem_create(Item* this, struct Furniture* furniture);

/* Java: FurnitureItem.getColor() */
PUBLIC int furnitureitem_get_color(Item* this);

/* Java: FurnitureItem.getSprite() */
PUBLIC int furnitureitem_get_sprite(Item* this);

/* Java: FurnitureItem.renderIcon(Screen, int, int) */
PUBLIC void furnitureitem_render_icon(Item* this, Screen* screen, int x, int y);

/* Java: FurnitureItem.renderInventory(Screen, int, int) */
PUBLIC void furnitureitem_render_inventory(Item* this, Screen* screen, int x, int y);

/*
 * Places the furniture on the target tile when that tile allows it.
 * Java: FurnitureItem.interactOn(Tile, Level, int, int, Player, int)
 */
PUBLIC boolean furnitureitem_interact_on(Item* this, TileID tile, Level* level, int xt, int yt, struct Player* player, int attackDir);

#endif /* FURNITURE_ITEM_H */
