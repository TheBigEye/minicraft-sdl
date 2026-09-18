/*
 * tooltype.h - The tool kinds (Java: com.mojang.ld22.item.ToolType).
 */
#ifndef TOOLTYPE_H
#define TOOLTYPE_H 1

#include "../utils/javalang.h"

/* The five tool kinds; the values double as sprite offsets. */
typedef enum ToolType {
    SHOVEL = 0,
    HOE = 1,
    SWORD = 2,
    PICKAXE = 3,
    AXE = 4
} ToolType;

/* Short display name for a tool kind. Java: ToolType.getName() */
PUBLIC const char* type_get_name(ToolType type);

#endif /* TOOLTYPE_H */
