/*
 * itemids.h - Item type tags for the Item union.
 */
#ifndef ITEMIDS_H
#define ITEMIDS_H

/* Discriminator selecting the active add.* payload in Item. */
typedef enum{
	FURNITURE,
	ITEM,
	POWERGLOVE,
	RESOURCE,
	TOOL
}ItemID;

#endif // ITEMIDS_H
