/*
 * anvil.h - The Anvil: crafting station for the anvil recipes
 *           (Java: com.mojang.ld22.entity.Anvil).
 */
#ifndef ANVIL_H
#define ANVIL_H 1

#include "furniture.h"

#include "../utils/javalang.h"

/* An anvil adds nothing to Furniture but its use(), so it is an alias. */
typedef Furniture Anvil;

/* Constructor. Java: Anvil() */
PUBLIC void anvil_create(Anvil* this);

/* Java: Anvil.use(Player, int) */
PUBLIC boolean anvil_use(Anvil* this, struct Player* player, int attackDir);

#endif /* ANVIL_H */
