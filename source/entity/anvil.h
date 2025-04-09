#ifndef ANVIL_H
#define ANVIL_H

#include "furniture.h"

typedef Furniture Anvil;
void anvil_create(Anvil* anvil);
char anvil_use(Anvil* anvil, struct _Player* player, int attackDir);

#endif // ANVIL_H
