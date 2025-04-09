#ifndef OVEN_H
#define OVEN_H
#include "furniture.h"

typedef Furniture Oven;

struct _Player;

void oven_create(Oven* oven);
char oven_use(Oven* oven, struct _Player* player, int attackDir);

#endif // OVEN_H
