/*
 * oven.h - The Oven: cooking station for the food recipes
 *          (Java: com.mojang.ld22.entity.Oven).
 */
#ifndef OVEN_H
#define OVEN_H 1

#include "furniture.h"

#include "../utils/javalang.h"

/* An oven adds nothing to Furniture but its use(), so it is an alias. */
typedef Furniture Oven;

struct Player;

/* Constructor. Java: Oven() */
PUBLIC void oven_create(Oven* this);

/* Java: Oven.use(Player, int) */
PUBLIC boolean oven_use(Oven* this, struct Player* player, int attackDir);

#endif /* OVEN_H */
