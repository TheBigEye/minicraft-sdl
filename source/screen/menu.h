/*
 * menu.h - The menu framework
 *          (Java: com.mojang.ld22.screen.Menu), the abstract base every
 *          screen extends.
 */
#ifndef SCREEN_MENU_H_
#define SCREEN_MENU_H_ 1

#include "../gfx/screen.h"
#include "../utils/javalang.h"
#include "../utils/arraylist.h"

typedef struct Menu Menu;

/*
 * The base class. Its three methods are the abstract ones Java declares in
 * `Menu`; each screen installs its own implementations in its constructor,
 * exactly like every other class of the port. There is no shared table of
 * operations: the pointers live on the instance.
 */
struct Menu {
    /* Java: Menu.tick() */
    void (*tick)(Menu* this);
    /* Java: Menu.render(Screen) */
    void (*render)(Menu* this, Screen* screen);
    /* Java: Menu.init(Game, InputHandler) */
    void (*init)(Menu* this);
};

/*
 * Identifies each screen. C-only: Java hands menu objects around by
 * reference, so it needs no ids at all.
 */
typedef enum menu_id {
    /* No menu: back to gameplay. Java: game.setMenu(null) */
    mid_NONE = 0,
    mid_TITLE = 1,
    mid_ABOUT,
    mid_CONTAINER,
    mid_CRAFTING,
    mid_DEAD,
    mid_INSTRUCTIONS,
    mid_LEVEL_TRANSITION,
    mid_WON,
    mid_INVENTORY
} menu_id;

/*
 * Menu to return to when a child screen is dismissed.
 *
 * Java: the parent Menu reference the child was constructed with.
 */
extern Menu* menu_parent;

/* Builds every screen and registers it under its id; call once. */
PUBLIC void init_menus(void);

/*
 * Looks a screen up by id; mid_NONE gives null.
 *
 * C-only: Java passes Menu references around, so it never resolves one.
 * The port needs it because `game_set_menu()` takes an id while the menus
 * are singletons built once by `init_menus()`.
 */
PUBLIC Menu* get_menu(menu_id id);

/*
 * Draws a scrollable list of items with a selection cursor; used by the
 * inventory, crafting and container menus.
 *
 * `callback` draws one row and has the shape of Java's
 * ListItem.renderInventory(Screen, int, int), with the row added as a
 * receiver in front.
 */
PUBLIC void menu_render_item_list(Screen* screen, int xo, int yo, int x1, int y1, ArrayList* listItems, int selected, void* callback);

#endif /* SCREEN_MENU_H_ */
