/*
 * furnace.h - The Furnace: smelting station, ores into ingots
 *             (Java: com.mojang.ld22.entity.Furnace).
 */
#ifndef FURNACE_H
#define FURNACE_H 1

#include "furniture.h"

#include "../utils/javalang.h"

/* A furnace adds nothing to Furniture but its use(), so it is an alias. */
typedef Furniture Furnace;

struct Player;

/* Constructor. Java: Furnace() */
PUBLIC void furnace_create(Furnace* this);

/* Java: Furnace.use(Player, int) */
PUBLIC boolean furnace_use(Furnace* this, struct Player* player, int attackDir);

#endif /* FURNACE_H */
