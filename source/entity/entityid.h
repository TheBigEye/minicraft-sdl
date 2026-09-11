/*
 * entityid.h - Runtime class tags for every entity type: the C
 *              replacement for Java's instanceof checks.
 */
#ifndef ENTITYID_H
#define ENTITYID_H

typedef enum{
	AIRWIZARD,
	ANVIL,
	CHEST,
	FURNACE,
	ITEMENTITY,
	LANTERN,
	OVEN,
	PLAYER,
	SLIME,
	SPARK,
	WORKBENCH,
	ZOMBIE,
	SMASHPARTICLE,
	TEXTPARTICLE
} EntityId;

#endif // ENTITYID_H
