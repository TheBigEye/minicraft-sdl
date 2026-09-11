/*
 * tool_item.h - Tool item (Java: ToolItem): shovel, hoe, sword,
 *               pickaxe and axe in five material tiers.
 */
#ifndef TOOL_ITEM_H
#define TOOL_ITEM_H

extern const int LEVEL_COLORS[]; /* Sprite color per material tier. */
extern const char* LEVEL_NAMES[]; /* Display name per material tier. */
extern const int MAX_LEVEL;

struct _Item;
struct _Screen;
struct _Entity;
enum _ToolType;

void toolitem_create(struct _Item* item, enum _ToolType type, int level);
int toolitem_getColor(struct _Item* item);
int toolitem_getSprite(struct _Item* item);
void toolitem_renderIcon(struct _Item* item, struct _Screen* screen, int x, int y);
void toolitem_renderInventory(struct _Item* item, struct _Screen* screen, int x, int y);
void toolitem_getName(struct _Item* item, char* buf);
/* Random attack damage bonus depending on tool type and level. */
int toolitem_getAttackDamageBonus(struct _Item* item, struct _Entity* e);
/* Tools only stack with identical type and level. */
char toolitem_matches(struct _Item* item, struct _Item* item2);


#endif // TOOL_ITEM_H
