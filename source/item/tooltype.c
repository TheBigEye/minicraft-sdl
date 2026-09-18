/*
 * tooltype.c - The tool kinds (Java: com.mojang.ld22.item.ToolType).
 */
#include "tooltype.h"

#include "../utils/javalang.h"


/* Java: ToolType.getName(). The short name shown after the material, so
 * that a level 2 pickaxe reads "Iron Pick". */
PUBLIC const char* type_get_name(ToolType type) {
    switch (type) {
        case SHOVEL:
            return "Shovel";
        case HOE:
            return "Hoe";
        case SWORD:
            return "Sword";
        case PICKAXE:
            return "Pick";
        case AXE:
            return "Axe";
        default:
            return "";
    }
}
