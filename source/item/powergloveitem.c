/*
 * powergloveitem.c - The power glove
 *                    (Java: com.mojang.ld22.item.PowerGloveItem).
 */
#include "powergloveitem.h"

#include <stdlib.h>
#include <string.h>

#include "../entity/furniture.h"
#include "../gfx/color.h"
#include "../gfx/font.h"
#include "itemids.h"


/* The glove has no per-instance state, so its name is shared. C-only. */
PRIVATE char name[] = "Pow glove";


/* Constructor. Java: PowerGloveItem() */
PUBLIC void powergloveitem_create(Item* this) {
    this->id = POWERGLOVE;
}


/* Java: PowerGloveItem.getColor() */
PUBLIC int powergloveitem_get_color(Item* this) {
    (void) this;

    return get_color4(-1, 100, 320, 430);
}


/* Java: PowerGloveItem.getSprite() */
PUBLIC int powergloveitem_get_sprite(Item* this) {
    (void) this;

    return 7 + 4 * 32;
}


/* Java: PowerGloveItem.renderIcon(Screen, int, int) */
PUBLIC void powergloveitem_render_icon(Item* this, Screen* screen, int x, int y) {
    screen->render(screen, x, y, powergloveitem_get_sprite(this), powergloveitem_get_color(this), 0);
}


/* Java: PowerGloveItem.renderInventory(Screen, int, int) */
PUBLIC void powergloveitem_render_inventory(Item* this, Screen* screen, int x, int y) {
    screen->render(screen, x, y, powergloveitem_get_sprite(this), powergloveitem_get_color(this), 0);
    font_draw(powergloveitem_get_name(this), strlen(name), screen, x + 8, y, get_color4(-1, 555, 555, 555));
}


/* Java: PowerGloveItem.getName() */
PUBLIC String powergloveitem_get_name(Item* this) {
    (void) this;

    return name;
}


/*
 * Java: PowerGloveItem.interact(Player, Entity, int).
 *
 * Punching furniture with the glove picks it up into the inventory;
 * anything else is ignored.
 */
PUBLIC boolean powergloveitem_interact(Item* this, Player* player, Entity* entity, int attackDir) {
    (void) this;
    (void) attackDir;

    if (entity_is_furniture(entity)) {
        furniture_take((Furniture *) entity, player);

        return true;
    }

    return false;
}
