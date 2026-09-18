/*
 * anvil.c - The Anvil furniture (Java: com.mojang.ld22.entity.Anvil).
 *
 * A placeable stationary entity; using it opens the crafting menu with
 * the anvil recipe list, that is, tool crafting and upgrades.
 */
#include "anvil.h"

#include <stdlib.h>
#include <string.h>

#include "../crafting/crafting.h"
#include "../game.h"
#include "../gfx/color.h"
#include "../screen/crafting_menu.h"


/*
 * Constructor: spawns an anvil, that is, furniture named "Anvil", sprite
 * 0, in an iron palette and with the small 3x2 collision box that all
 * crafting stations share.
 */
PUBLIC void anvil_create(Anvil* this) {
    /* XXX ew: the name has to outlive this call, so it is heap-allocated. */
    String name = new_array(char, strlen("Anvil") + 1);

    strcpy(name, "Anvil");

    furniture_create(this, name);

    /* Java: class Anvil extends Furniture */
    this->entity.use = (entity_use_fn) anvil_use;

    this->entity.type = ANVIL;
    this->col = get_color4(-1, 000, 111, 222);
    this->sprite = 0;
    this->entity.xr = 3;
    this->entity.yr = 2;
}


/* Opens the crafting menu on the anvil recipe list; it always succeeds. */
PUBLIC boolean anvil_use(Anvil* this, struct Player* player, int attackDir) {
    (void) this;
    (void) player;
    (void) attackDir;

    crmenu_recipes = &anvilRecipes;
    game_set_menu(mid_CRAFTING);

    return true;
}
