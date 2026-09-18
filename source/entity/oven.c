/*
 * oven.c - The Oven furniture (Java: com.mojang.ld22.entity.Oven).
 *
 * A placeable stationary entity; using it opens the crafting menu with the
 * oven recipe list, that is, cooking food.
 */
#include "oven.h"

#include <stdlib.h>
#include <string.h>

#include "../crafting/crafting.h"
#include "../game.h"
#include "../gfx/color.h"
#include "../screen/crafting_menu.h"
#include "furniture.h"


/*
 * Constructor: spawns an oven, that is, furniture named "Oven", sprite 2,
 * in a baked-clay palette and with the small 3x2 collision box of the
 * crafting stations.
 */
PUBLIC void oven_create(Oven* this) {
    /* XXX ew: the name has to outlive this call, so it is heap-allocated. */
    String name = new_array(char, strlen("Oven") + 1);

    strcpy(name, "Oven");

    furniture_create(this, name);

    /* Java: class Oven extends Furniture */
    this->entity.use = (entity_use_fn) oven_use;

    this->entity.type = OVEN;
    this->col = get_color4(-1, 000, 332, 442);
    this->sprite = 2;
    this->entity.xr = 3;
    this->entity.yr = 2;
}


/* Opens the crafting menu on the oven recipe list; it always succeeds. */
PUBLIC boolean oven_use(Oven* this, struct Player* player, int attackDir) {
    (void) this;
    (void) player;
    (void) attackDir;

    crmenu_recipes = &ovenRecipes;
    game_set_menu(mid_CRAFTING);

    return true;
}
