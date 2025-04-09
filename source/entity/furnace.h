#ifndef FURNACE_H
#define FURNACE_H
#include "furniture.h"
typedef Furniture Furnace;

struct _Player;

void furnace_create(Furnace* furnace);
char furnace_use(Furnace* furnace, struct _Player* player, int attackDir);

#endif // FURNACE_H
