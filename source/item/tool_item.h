#ifndef TOOL_ITEM_H
#define TOOL_ITEM_H

extern const int LEVEL_COLORS[];
extern const char* LEVEL_NAMES[];
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
int toolitem_getAttackDamageBonus(struct _Item* item, struct _Entity* e);
char toolitem_matches(struct _Item* item, struct _Item* item2);


#endif // TOOL_ITEM_H
