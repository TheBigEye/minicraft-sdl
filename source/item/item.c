/*
 * item.c - The Item base dispatch (Java: com.mojang.ld22.item.Item).
 *
 * Each operation switches on the item id and forwards to the matching type
 * module (resource, tool, furniture or power glove), which is where the
 * per-subclass overrides of the Java original live.
 */
#include "item.h"

#include <stdlib.h>
#include <string.h>

#include "../entity/furniture.h"
#include "furniture_item.h"
#include "powergloveitem.h"
#include "resourceitem.h"
#include "tool_item.h"


/* Java: Item.getColor(). Returns the item's sprite color. */
PUBLIC int item_get_color(Item* this) {
    switch (this->id) {
        case POWERGLOVE:
            return powergloveitem_get_color(this);
        case RESOURCE:
            return resourceitem_get_color(this);
        case TOOL:
            return toolitem_get_color(this);
        case FURNITURE:
            return furnitureitem_get_color(this);
        default:
            return 0;
    }
}


/* Java: Item.getSprite(). Returns the item's sprite index. */
PUBLIC int item_get_sprite(Item* this) {
    switch (this->id) {
        case POWERGLOVE:
            return powergloveitem_get_sprite(this);
        case RESOURCE:
            return resourceitem_get_sprite(this);
        case TOOL:
            return toolitem_get_sprite(this);
        case FURNITURE:
            return furnitureitem_get_sprite(this);
        default:
            return 0;
    }
}


/* Java: Item.onTake(ItemEntity). Nothing to do here, and neither of the
 * subclasses overrides it either. */
PUBLIC void item_on_take(Item* this, struct ItemEntity* itemEntity) {
    (void) this;
    (void) itemEntity;
}


/* Java: Item.renderInventory(Screen, int, int). Draws the icon plus label
 * row shown in the inventory menu. */
PUBLIC void item_render_inventory(Item* this, Screen* screen, int x, int y) {
    switch (this->id) {
        case POWERGLOVE:
            powergloveitem_render_inventory(this, screen, x, y);
            break;
        case RESOURCE:
            resourceitem_render_inventory(this, screen, x, y);
            break;
        case TOOL:
            toolitem_render_inventory(this, screen, x, y);
            break;
        case FURNITURE:
            furnitureitem_render_inventory(this, screen, x, y);
            break;
        default:
            break;
    }
}


/* Java: Item.renderIcon(Screen, int, int). Draws the small icon used in the
 * HUD and the crafting lists. */
PUBLIC void item_render_icon(Item* this, Screen* screen, int x, int y) {
    switch (this->id) {
        case POWERGLOVE:
            powergloveitem_render_icon(this, screen, x, y);
            break;
        case RESOURCE:
            resourceitem_render_icon(this, screen, x, y);
            break;
        case TOOL:
            toolitem_render_icon(this, screen, x, y);
            break;
        case FURNITURE:
            furnitureitem_render_icon(this, screen, x, y);
            break;
        default:
            break;
    }
}


/* Java: Item.interact(Player, Entity, int). Interaction aimed at an entity;
 * only the power glove uses it. */
PUBLIC boolean item_interact(Item* this, struct Player* player, Entity* entity, int attackDir) {
    switch (this->id) {
        case POWERGLOVE:
            return powergloveitem_interact(this, player, entity, attackDir);
        default:
            return false;
    }
}


/* Java: Item.interactOn(Tile, Level, int, int, Player, int). Interaction
 * aimed at a tile: furniture placement, planting, and so on. */
PUBLIC boolean item_interact_on(Item* this, TileID tile, Level* level, int xt, int yt, struct Player* player, int attackDir) {
    switch (this->id) {
        case FURNITURE:
            return furnitureitem_interact_on(this, tile, level, xt, yt, player, attackDir);
        case RESOURCE:
            return resourceitem_interact_on(this, tile, level, xt, yt, player, attackDir);
        default:
            return false;
    }
}


/* Java: Item.isDepleted(). Resources deplete at zero count, furniture once
 * placed. */
PUBLIC boolean item_is_depleted(Item* this) {
    switch (this->id) {
        case RESOURCE:
            return this->add.resource.count <= 0;
        case FURNITURE:
            return this->add.furniture.placed;
        default:
            return false;
    }
}


/* Java: Item.canAttack(). Only tools can attack entities. */
PUBLIC boolean item_can_attack(Item* this) {
    switch (this->id) {
        case TOOL:
            return true;
        case FURNITURE:
            return false;
        default:
            return false;
    }
}


/* Java: Item.getAttackDamageBonus(Entity). Only tools add a bonus. */
PUBLIC int item_get_attack_damage_bonus(Item* this, Entity* entity) {
    if (this->id == TOOL) {
        return toolitem_get_attack_damage_bonus(this, entity);
    }

    return 0;
}


/* Java: Item.getName(). Copies the display name of the active type into
 * `buf`. */
PUBLIC void item_get_name(Item* this, char* buf) {
    switch (this->id) {
        case FURNITURE:
            strcpy(buf, this->add.furniture.furniture->name);
            break;
        case POWERGLOVE:
            strcpy(buf, powergloveitem_get_name(this));
            break;
        case RESOURCE:
            strcpy(buf, this->add.resource.resource->name);
            break;
        case TOOL:
            toolitem_get_name(this, buf);
            break;
        default:
            break;
    }
}


/*
 * Java: Item.matches(Item). Tools compare type and level through their own
 * matcher; every other kind stacks whenever the ids match.
 *
 * XXX vanilla bug carried over on purpose: comparing ids is not enough, so
 * two different resources of the same id still stack.
 */
PUBLIC boolean item_matches(Item* this, Item* item) {
    if (this->id == TOOL) return toolitem_matches(this, item);

    return this->id == item->id;
}


/*
 * C-only: releases the type-specific storage. A furniture item still
 * carrying its entity frees it; placed furniture and the other kinds hold
 * nothing of their own.
 */
PUBLIC void item_free(Item* this) {
    switch (this->id) {
        case FURNITURE:
            if (this->add.furniture.furniture) {
                Entity* furn = (Entity *) this->add.furniture.furniture;

                furn->free(furn);
                delete(this->add.furniture.furniture);
            }
            break;
        case ITEM:
        case RESOURCE:
        case TOOL:
            break;
        default:
            break;
    }
}
