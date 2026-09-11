/*
 * workbench.h - Workbench furniture: the main crafting station.
 */
#ifndef WORKBENCH_H
#define WORKBENCH_H
#include "furniture.h"

typedef Furniture Workbench;

void workbench_create(Workbench* workbench);
char workbench_use(Workbench* workbench, struct _Player* player, int attackDir);

#endif // WORKBENCH_H
