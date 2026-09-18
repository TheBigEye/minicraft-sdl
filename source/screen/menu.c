/*
 * menu.c - The menu framework
 *          (Java: com.mojang.ld22.screen.Menu).
 *
 * Every screen is one statically allocated instance whose constructor is
 * called once by init_menus(); the id is only a handle to reach it.
 */
#include "menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../gfx/color.h"
#include "../gfx/font.h"
#include "../gfx/screen.h"
#include "../log.h"
#include "../utils/arraylist.h"
#include "list_item.h"

#include "about_menu.h"
#include "container_menu.h"
#include "crafting_menu.h"
#include "dead_menu.h"
#include "instructions_menu.h"
#include "inventory_menu.h"
#include "level_transition_menu.h"
#include "title_menu.h"
#include "won_menu.h"


#define MENUS_SIZE 16

/* The registry: one screen per id. */
PRIVATE Menu* menus[MENUS_SIZE];

Menu* menu_parent = null;


/*
 * Builds each screen and registers it under its id.
 *
 * Java has no equivalent: it constructs a new menu every time one is
 * opened. The port keeps one instance of each, so they are built once here.
 */
PUBLIC void init_menus(void) {
    for (int i = 0; i < MENUS_SIZE; ++i) {
        menus[i] = null;
    }

    titlemenu_create(&titlemenu);
    aboutmenu_create(&aboutmenu);
    instructionsmenu_create(&instructionsmenu);
    inventorymenu_create(&inventorymenu);
    deadmenu_create(&deadmenu);
    leveltransitionmenu_create(&leveltransitionmenu);
    craftingmenu_create(&craftingmenu);
    containermenu_create(&containermenu);
    wonmenu_create(&wonmenu);

    menus[mid_TITLE] = &titlemenu.menu;
    menus[mid_ABOUT] = &aboutmenu.menu;
    menus[mid_INSTRUCTIONS] = &instructionsmenu.menu;
    menus[mid_INVENTORY] = &inventorymenu.menu;
    menus[mid_DEAD] = &deadmenu.menu;
    menus[mid_LEVEL_TRANSITION] = &leveltransitionmenu.menu;
    menus[mid_CRAFTING] = &craftingmenu.menu;
    menus[mid_CONTAINER] = &containermenu.menu;
    menus[mid_WON] = &wonmenu.menu;
}


PUBLIC Menu* get_menu(menu_id id) {
    if ((int) id < 0 || id >= MENUS_SIZE) {
        LOG_ERROR("menu id out of range(%d)", id);
        return null;
    }

    return menus[id];
}


/*
 * Draws a window of list items centred on the selection, with the > and <
 * cursors; a negative `selected` hides the cursor, which is how the
 * container menu dims the pane that is not focused. Items are drawn through
 * the per-row callback.
 */
PUBLIC void menu_render_item_list(Screen* screen, int xo, int yo, int x1, int y1, ArrayList* listItems, int selected, void* callback) {
    boolean renderCursor = true;

    if (selected < 0) {
        selected = -selected - 1;
        renderCursor = false;
    }

    int w = x1 - xo;
    int h = y1 - yo - 1;
    int i0 = 0;
    int i1 = listItems->size;

    if (i1 > h) i1 = h;

    int io = selected - h / 2;

    if (io > listItems->size - h) {
        io = listItems->size - h;
    }

    if (io < 0) io = 0;

    for (int i = i0; i < i1; ++i) {
        void* item = listItems->get(listItems, i + io);

        ((void (*) (void*, Screen*, int, int)) (callback))(item, screen, (1 + xo) * 8, (i + 1 + yo) * 8);
    }

    if (renderCursor) {
        int yy = selected + 1 - io + yo;

        font_draw(">", 1, screen, (xo + 0) * 8, yy * 8, get_color4(5, 555, 555, 555));
        font_draw("<", 1, screen, (xo + w) * 8, yy * 8, get_color4(5, 555, 555, 555));
    }
}
