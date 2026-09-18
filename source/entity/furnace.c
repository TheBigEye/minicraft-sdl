/*
 * furnace.c - The Furnace furniture
 *             (Java: com.mojang.ld22.entity.Furnace).
 *
 * A placeable stationary entity; using it opens the crafting menu with the
 * furnace recipe list, that is, smelting ore into metal.
 */
#include "furnace.h"

#include <stdlib.h>
#include <string.h>

#include "../crafting/crafting.h"
#include "../game.h"
#include "../gfx/color.h"
#include "../screen/crafting_menu.h"


/*
 * Constructor: spawns a furnace, that is, furniture named "Furnace",
 * sprite 3, in a stone and fire palette and with the small 3x2 collision
 * box of the crafting stations.
 */
PUBLIC void furnace_create(Furnace* this) {
    /* XXX ew: the name has to outlive this call, so it is heap-allocated. */
    String name = new_array(char, strlen("Furnace") + 1);

    strcpy(name, "Furnace");

    furniture_create(this, name);

    /* Java: class Furnace extends Furniture */
    this->entity.use = (entity_use_fn) furnace_use;

    this->entity.type = FURNACE;
    this->col = get_color4(-1, 000, 222, 333);
    this->sprite = 3;
    this->entity.xr = 3;
    this->entity.yr = 2;
}


/* Opens the crafting menu on the furnace recipe list; it always succeeds. */
PUBLIC boolean furnace_use(Furnace* this, struct Player* player, int attackDir) {
    (void) this;
    (void) player;
    (void) attackDir;

    crmenu_recipes = &furnaceRecipes;
    game_set_menu(mid_CRAFTING);

    return true;
}
