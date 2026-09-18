/*
 * tool_item.h - The tool item (Java: com.mojang.ld22.item.ToolItem):
 *               shovel, hoe, sword, pickaxe and axe, in five material
 *               tiers.
 */
#ifndef TOOL_ITEM_H
#define TOOL_ITEM_H 1

#include "../utils/javalang.h"
#include "item.h"
#include "tooltype.h"

/* Sprite color per material tier.
 * Java: `private static final int[] LEVEL_COLORS` */
extern const int LEVEL_COLORS[];
/* Display name per material tier.
 * Java: `private static final String[] LEVEL_NAMES` */
extern const char* LEVEL_NAMES[];
/* How many material tiers there are. Java: `public static final int MAX_LEVEL` */
extern const int MAX_LEVEL;

/* Constructor: Java: ToolItem(ToolType type, int level) */
PUBLIC void toolitem_create(Item* this, ToolType type, int level);

/* Java: ToolItem.getColor() */
PUBLIC int toolitem_get_color(Item* this);

/* Java: ToolItem.getSprite() */
PUBLIC int toolitem_get_sprite(Item* this);

/* Java: ToolItem.renderIcon(Screen, int, int) */
PUBLIC void toolitem_render_icon(Item* this, Screen* screen, int x, int y);

/* Java: ToolItem.renderInventory(Screen, int, int) */
PUBLIC void toolitem_render_inventory(Item* this, Screen* screen, int x, int y);

/*
 * Composes "Material Type", for example "Iron Pick".
 * Java: ToolItem.getName(), which returns a String; the port writes into
 * `buf` instead, so that no caller has to free it.
 */
PUBLIC void toolitem_get_name(Item* this, char* buf);

/* Random attack damage bonus depending on tool type and level.
 * Java: ToolItem.getAttackDamageBonus(Entity) */
PUBLIC int toolitem_get_attack_damage_bonus(Item* this, Entity* e);

/* Tools only stack with an identical type and level.
 * Java: ToolItem.matches(Item) */
PUBLIC boolean toolitem_matches(Item* this, Item* item);

#endif /* TOOL_ITEM_H */
