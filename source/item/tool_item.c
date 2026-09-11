/*
 * tool_item.c - Tool item behavior (Java: ToolItem).
 */
#include "tool_item.h"
#include <stdio.h>
#include <stdlib.h>
#include "../gfx/color.h"
#include "../gfx/screen.h"
#include "../gfx/font.h"
#include "item.h"
#include "tooltype.h"
#include "../entity/entity.h"

#include <string.h>

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


/* Tags the item as a tool of the given type and material level,
 * seeding its bonus-roll RNG from the clock. */
void toolitem_create(Item* item, ToolType type, int level){
	// item_create(item);
	item->id = TOOL;
	random_set_seed(&item->add.tool.random, getTimeMS());
	item->add.tool.type = type;
	item->add.tool.level = level;
}


/* Color comes from the material tier table. */
int toolitem_getColor(Item* item) {
	return LEVEL_COLORS[item->add.tool.level];
}


/* Each tool type occupies one sprite in the tool row. */
int toolitem_getSprite(Item* item) {
	return item->add.tool.type + 5 * 32;
}


void toolitem_renderIcon(Item* item, Screen* screen, int x, int y) {
	render_screen(screen, x, y, toolitem_getSprite(item), toolitem_getColor(item), 0);
}


void toolitem_renderInventory(Item* item, Screen* screen, int x, int y) {
	render_screen(screen, x, y, toolitem_getSprite(item), toolitem_getColor(item), 0);
	char* name = malloc(32); //XXX ew
	toolitem_getName(item, name);
	font_draw(name, strlen(name), screen, x + 8, y, GETCOLOR4(-1, 555, 555, 555));
	free(name);
}


/* Composes "Material Type", e.g. "Iron Pick". */
void toolitem_getName(Item* item, char* buf) {
	sprintf(buf, "%s %s", LEVEL_NAMES[item->add.tool.level], type_getName(item->add.tool.type));
}


/* Axes and swords add level-scaled random damage; other tools add
 * the bare minimum. */
int toolitem_getAttackDamageBonus(Item* item, Entity* e) {
	int lvl = item->add.tool.level;
	Random* rand = &item->add.tool.random;

	// printf("%d %d\n", item->add.tool.type, lvl);

	switch (item->add.tool.type) {
		case AXE:
			return (lvl + 1) * 2 + random_next_int(rand, 4);
		case SWORD:
			return (lvl + 1) * 3 + random_next_int(rand, 2 + lvl * lvl * 2);
		default:
			return 1;
	}
}


/* Only an identical tool (same type and level) matches. */
char toolitem_matches(Item* item, Item* item2){
	if (item2->id == TOOL) {
		if (item2->add.tool.type != item->add.tool.type) return 0;
		if (item2->add.tool.level != item->add.tool.level) return 0;
		return 1;
	}
	return 0;
}
