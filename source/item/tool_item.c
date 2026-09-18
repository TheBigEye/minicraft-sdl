/*
 * tool_item.c - The tool item (Java: com.mojang.ld22.item.ToolItem).
 */
#include "tool_item.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../gfx/color.h"
#include "../gfx/font.h"
#include "../gfx/screen.h"
#include "../log.h"
#include "../utils/utils.h"
#include "item.h"
#include "tooltype.h"

#include "../entity/entity.h"


const int MAX_LEVEL = 5;

const int LEVEL_COLORS[] = {
    GETCOLOR4(-1, 100, 321, 431),
    GETCOLOR4(-1, 100, 321, 111),
    GETCOLOR4(-1, 100, 321, 555),
    GETCOLOR4(-1, 100, 321, 550),
    GETCOLOR4(-1, 100, 321, 055)
};

const char* LEVEL_NAMES[] = {
    "Wood",
    "Rock",
    "Iron",
    "Gold",
    "Gem"
};


/*
 * Constructor: tags the item as a tool of the given type and material
 * level, seeding its bonus-roll RNG from the clock.
 * Java: ToolItem(ToolType type, int level)
 */
PUBLIC void toolitem_create(Item* this, ToolType type, int level) {
    this->id = TOOL;

    /* C-only: each tool carries its own generator, seeded from the clock. */
    random_create(&this->add.tool.random);
    random_set_seed(&this->add.tool.random, get_time_ms());

    this->add.tool.type = type;
    this->add.tool.level = level;
}


/* Java: ToolItem.getColor(). The color comes from the material tier table. */
PUBLIC int toolitem_get_color(Item* this) {
    return LEVEL_COLORS[this->add.tool.level];
}


/* Java: ToolItem.getSprite(). Each tool type occupies one sprite in the
 * tool row. */
PUBLIC int toolitem_get_sprite(Item* this) {
    return this->add.tool.type + 5 * 32;
}


/* Java: ToolItem.renderIcon(Screen, int, int) */
PUBLIC void toolitem_render_icon(Item* this, Screen* screen, int x, int y) {
    screen->render(screen, x, y, toolitem_get_sprite(this), toolitem_get_color(this), 0);
}


/* Java: ToolItem.renderInventory(Screen, int, int) */
PUBLIC void toolitem_render_inventory(Item* this, Screen* screen, int x, int y) {
    screen->render(screen, x, y, toolitem_get_sprite(this), toolitem_get_color(this), 0);

    /* XXX ew: the name has to outlive the call, so it is heap-allocated. */
    String name = new_array(char, 32);

    toolitem_get_name(this, name);
    font_draw(name, strlen(name), screen, x + 8, y, GETCOLOR4(-1, 555, 555, 555));

    delete(name);
}


/* Java: ToolItem.getName(). Composes "Material Type", e.g. "Iron Pick". */
PUBLIC void toolitem_get_name(Item* this, char* buf) {
    sprintf(buf, "%s %s", LEVEL_NAMES[this->add.tool.level], type_get_name(this->add.tool.type));
}


/*
 * Java: ToolItem.getAttackDamageBonus(Entity).
 *
 * Axes and swords add level-scaled random damage; every other tool adds the
 * bare minimum. `e` is part of the Java signature but the bonus does not
 * depend on it.
 */
PUBLIC int toolitem_get_attack_damage_bonus(Item* this, Entity* e) {
    int lvl = this->add.tool.level;
    Random* rand = &this->add.tool.random;

    (void) e;

    LOG_TRACE("tool attack bonus: type %d, level %d", this->add.tool.type, lvl);

    switch (this->add.tool.type) {
        case AXE:
            return (lvl + 1) * 2 + rand->next_int(rand, 4);
        case SWORD:
            return (lvl + 1) * 3 + rand->next_int(rand, 2 + lvl * lvl * 2);
        default:
            return 1;
    }
}


/* Java: ToolItem.matches(Item). Only an identical tool (same type and
 * level) matches. */
PUBLIC boolean toolitem_matches(Item* this, Item* item) {
    if (item->id != TOOL) return false;
    if (item->add.tool.type != this->add.tool.type) return false;
    if (item->add.tool.level != this->add.tool.level) return false;

    return true;
}
