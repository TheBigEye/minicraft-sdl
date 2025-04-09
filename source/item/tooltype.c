#include "tooltype.h"
#include <stdio.h>

char* type_getName(ToolType type){
	switch(type){
		case SHOVEL: return "Shvl";
		case HOE: return "Hoe";
		case SWORD: return "Swrd";
		case PICKAXE: return "Pick";
		case AXE: return "Axe";
		default:
			printf("Unknown tool %d!\n", type);
			return 0;
	}
}
