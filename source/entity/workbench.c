/*
 * workbench.c - The Workbench furniture
 *               (Java: com.mojang.ld22.entity.Workbench).
 *
 * The basic crafting station of the game; using it opens the crafting menu
 * with the workbench recipe list, that is, tools and furniture.
 */
#include "workbench.h"

#include <stdlib.h>
#include <string.h>

#include "../crafting/crafting.h"
#include "../game.h"
#include "../gfx/color.h"
#include "../screen/crafting_menu.h"


/*
 * Constructor: spawns a workbench, that is, furniture named "Workbench",
 * sprite 4, in a wooden palette and with the small 3x2 collision box of
 * the crafting stations.
 */
PUBLIC void workbench_create(Workbench* this) {
    /* XXX ew: the name has to outlive this call, so it is heap-allocated. */
    String name = new_array(char, strlen("Workbench") + 1);

    strcpy(name, "Workbench");

    furniture_create(this, name);

    /* Java: class Workbench extends Furniture */
    this->entity.use = (entity_use_fn) workbench_use;

    this->entity.type = WORKBENCH;
    this->col = get_color4(-1, 100, 321, 431);
    this->sprite = 4;
    this->entity.xr = 3;
    this->entity.yr = 2;
}


/* Opens the crafting menu on the workbench recipe list; it always succeeds. */
PUBLIC boolean workbench_use(Workbench* this, struct Player* player, int attackDir) {
    (void) this;
    (void) player;
    (void) attackDir;

    crmenu_recipes = &workbenchRecipes;
    game_set_menu(mid_CRAFTING);

    return true;
}
