/*
 * tooltype.h - Tool type tags (Java: ToolType).
 */
#ifndef TOOLTYPE_H
#define TOOLTYPE_H

/* The five tool kinds; values double as sprite offsets. */
typedef enum _ToolType{
	SHOVEL = 0,
	HOE = 1,
	SWORD = 2,
	PICKAXE = 3,
	AXE = 4
} ToolType;

/* Short display name for a tool type. */
char* type_getName(ToolType type);

#endif // TOOLTYPE_H
