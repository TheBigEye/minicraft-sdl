/*
 * arraylist.h - Growable array of void* pointers.
 *
 * C stand-in for java.util.ArrayList, used all over the port: entity
 * lists, inventories and crafting recipes all live in these.
 */
#ifndef UTILS_ARRAYLIST_H_
#define UTILS_ARRAYLIST_H_ 1

typedef struct{
	int capacity;    /* allocated slots in `elements` */
	int size;        /* slots currently in use */
	void** elements; /* backing store, grown with realloc() on demand */
} ArrayList;

/* Resets the list to empty; no memory is allocated until the first push. */
void create_arraylist(ArrayList* list);
/* Inserts `element` at position `index`, shifting the tail right. */
void arraylist_pushTo(ArrayList* list, int index, void* element);
/* Appends `element` at the tail, growing the backing store if full. */
void arraylist_push(ArrayList* list, void* element);
/* Removes and returns the last element (does not shrink the store). */
void* arraylist_pop(ArrayList* list);
/* Removes the first slot holding `element`; warns if not found. */
void* arraylist_removeElement(ArrayList* list, void* element);
/* Removes the slot at position `index`, shifting the tail left. */
void* arraylist_removeId(ArrayList* list, int index);
/* Element at position `index`; bounds checking is the caller's job. */
void* arraylist_get(ArrayList* list, int index);
/* free()s every element and then the store; for lists owning their elements. */
void arraylist_remove_and_dealloc_each(ArrayList* list);
/* Frees only the backing store; the elements themselves are owned elsewhere. */
void arraylist_remove(ArrayList* list);
/* Empties the list logically, keeping the allocation for reuse. */
void arraylist_clear(ArrayList* list);

#endif /* UTILS_ARRAYLIST_H_ */
