/*
 * crafting_menu.c - The crafting menu
 *                   (Java: com.mojang.ld22.screen.CraftingMenu).
 */
#include "crafting_menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../crafting/recipe.h"
#include "../entity/inventory.h"
#include "../entity/player.h"
#include "../game.h"
#include "../gfx/color.h"
#include "../gfx/font.h"
#include "../inputhandler.h"
#include "../item/item.h"
#include "../sound/sound.h"
#include "menu.h"


ArrayList* crmenu_recipes = null;

CraftingMenu craftingmenu;


/* Java: new CraftingMenu(). Installs the methods and selects the first row. */
PUBLIC void craftingmenu_create(CraftingMenu* this) {
    this->selected = 0;

    this->menu.tick = craftingmenu_tick;
    this->menu.render = craftingmenu_render;
    this->menu.init = craftingmenu_init;
}


/*
 * Java: CraftingMenu.tick().
 *
 * Up and down cycle the recipe list, wrapping around; confirming crafts the
 * selected recipe when it is affordable, and then refreshes the
 * availability of the whole list.
 */
PUBLIC void craftingmenu_tick(Menu* this) {
    CraftingMenu* craft = (CraftingMenu*) this;

    if (menu.clicked) game_set_menu(null);
    if (up.clicked) --craft->selected;
    if (down.clicked) ++craft->selected;

    int len = crmenu_recipes->size;

    if (len == 0) craft->selected = 0;
    if (craft->selected < 0) craft->selected += len;
    if (craft->selected >= len) craft->selected -= len;

    if (attack.clicked && len > 0) {
        Recipe* r = crmenu_recipes->elements[craft->selected];

        recipe_check_can_craft(r, game_player);

        if (r->canCraft) {
            recipe_deduct_cost(r, game_player);
            recipe_craft(r, game_player);
            sound_play(SND_CRAFT);   /* Java: Sound.craft.play() */
        }

        for (int i = 0; i < crmenu_recipes->size; ++i) {
            recipe_check_can_craft(crmenu_recipes->elements[i], game_player);
        }
    }
}


/* Sorts the craftable recipes ahead of the unaffordable ones. */
PRIVATE int craftingmenu_cmp(const void* recipe, const void* recipe2) {
    Recipe* r1 = *(Recipe**) recipe;
    Recipe* r2 = *(Recipe**) recipe2;

    if (r1->canCraft && !r2->canCraft) return -1;
    if (!r1->canCraft && r2->canCraft) return 1;

    return 0;
}


/*
 * Java: CraftingMenu.init().
 *
 * Re-checks every recipe against the inventory and sorts the craftable ones
 * first.
 */
PUBLIC void craftingmenu_init(Menu* this) {
    ((CraftingMenu*) this)->selected = 0;

    for (int i = 0; i < crmenu_recipes->size; ++i) {
        recipe_check_can_craft(crmenu_recipes->elements[i], game_player);
    }

    qsort(crmenu_recipes->elements, crmenu_recipes->size, sizeof(*crmenu_recipes->elements), craftingmenu_cmp);
}


/*
 * Java: CraftingMenu.render(Screen).
 *
 * Draws the recipe list plus the "Have" and "Cost" panels; a cost is dimmed
 * while the player is short of that resource.
 */
PUBLIC void craftingmenu_render(Menu* this, Screen* screen) {
    CraftingMenu* craft = (CraftingMenu*) this;

    char s1[] = "Have";
    char s2[] = "Cost";
    char s3[] = "Crafting";
    char buf[64] = {0};

    font_render_frame(screen, s1, strlen(s1), 14, 1, 21, 3);
    font_render_frame(screen, s2, strlen(s2), 14, 4, 21, 11);
    font_render_frame(screen, s3, strlen(s3), 0, 1, 13, 11);

    menu_render_item_list(screen, 0, 1, 13, 11, crmenu_recipes, craft->selected, recipe_render_inventory);

    if (crmenu_recipes->size > 0) {
        Recipe* recipe = crmenu_recipes->elements[craft->selected];
        Item* result = &recipe->resultTemplate;
        int hasResultItems = inventory_count(&game_player->inventory, result);
        int xo = 15 * 8;

        screen->render(screen, xo, 2 * 8, item_get_sprite(result), item_get_color(result), 0);

        sprintf(buf, "%d", hasResultItems);
        font_draw(buf, strlen(buf), screen, xo + 8, 2 * 8, get_color4(-1, 555, 555, 555));

        for (int i = 0; i < recipe->costs.size; ++i) {
            Item* item = recipe->costs.elements[i];
            int yo = (5 + i) * 8;

            screen->render(screen, xo, yo, item_get_sprite(item), item_get_color(item), 0);

            int requiredAmt = 1;

            if (item->id == RESOURCE) {
                requiredAmt = item->add.resource.count;
            }

            int has = inventory_count(&game_player->inventory, item);
            int color = get_color4(-1, 555, 555, 555);

            if (has < requiredAmt) color = get_color4(-1, 222, 222, 222);
            if (has > 99) has = 99;

            sprintf(buf, "%d/%d", requiredAmt, has);
            font_draw(buf, strlen(buf), screen, xo + 8, yo, color);
        }
    }
}
