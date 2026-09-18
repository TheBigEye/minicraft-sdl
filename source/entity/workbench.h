/*
 * workbench.h - The Workbench: the main crafting station
 *               (Java: com.mojang.ld22.entity.Workbench).
 */
#ifndef WORKBENCH_H
#define WORKBENCH_H 1

#include "furniture.h"

#include "../utils/javalang.h"

/* A workbench adds nothing to Furniture but its use(), so it is an alias. */
typedef Furniture Workbench;

/* Constructor. Java: Workbench() */
PUBLIC void workbench_create(Workbench* this);

/* Java: Workbench.use(Player, int) */
PUBLIC boolean workbench_use(Workbench* this, struct Player* player, int attackDir);

#endif /* WORKBENCH_H */
