/*
 * arraylist.c - Dynamic pointer array backing the game's list semantics.
 *
 * Growth is manual (realloc on demand) and element order is preserved on
 * insert/remove, matching the Java ArrayList behavior the game logic
 * relies on (e.g. entities added mid-tick are visited next tick).
 */
#include "arraylist.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/* Initializes an empty list; the backing store stays NULL until first use. */
void create_arraylist(ArrayList* list){
	list->capacity = 0;
	list->size = 0;
	list->elements = 0;
}


/*
 * Inserts `element` at `index`, moving the tail one slot right.
 * Appending at the tail degenerates to arraylist_push(); an index beyond
 * the tail is rejected because it would leave a hole in the store.
 */
void arraylist_pushTo(ArrayList* list, int index, void* element){
	if (index > list->size) {
		printf("Tried pushing element too far!\n");
		return;

	} else if(index == list->size) {
		arraylist_push(list, element);

	} else {
		int size = list->size + 1;

		if (size > list->capacity) {
			list->elements = realloc(list->elements, sizeof(void*) * size);
		}

		for (int i = list->size; i > index; --i) {
			list->elements[i] = list->elements[i-1];
		}

		list->elements[index] = element;
		list->size = size;
	}
}


/* Appends `element`, allocating or growing the backing store when full. */
void arraylist_push(ArrayList* list, void* element) {
	int size = list->size + 1;

	if (size > list->capacity) {
		if (list->elements) {
			list->elements = realloc(list->elements, sizeof(void*) * size);

		} else {
			list->elements = malloc(sizeof(void*) * size);
		}

		list->capacity = size;
	}

	list->size = size;
	list->elements[list->size - 1] = element;
}


/* Element at `index`; no bounds checking, the caller guarantees validity. */
void* arraylist_get(ArrayList* list, int index) {
	return list->elements[index];
}


/* Finds the first slot holding `element` and removes it, else warns. */
void* arraylist_removeElement(ArrayList* list, void* element) {
	for (int i = 0; i < list->size; ++i) {
		void* e = list->elements[i];

		if (e == element) {
			return arraylist_removeId(list, i);
		}
	}

	printf("Tried removing unknown element from array (%p)!\n", element);
	return 0;
}


/* Removes the slot at `index`, closing the gap by shifting the tail left. */
void* arraylist_removeId(ArrayList* list, int index) {
	void* element = list->elements[index];

	if (index < list->size-1) {
		for(int i = index + 1; i < list->size; ++i) {
			list->elements[i-1] = list->elements[i];
		}
	}

	--list->size;
	return element;
}


/* Drops the last element and returns it; the store keeps its capacity. */
void* arraylist_pop(ArrayList* list) {
	--list->size;
	void* elem = list->elements[list->size];
	return elem;
}


/* Frees every element and then the store; the list must not be reused. */
void arraylist_remove_and_dealloc_each(ArrayList* list) {
	for (int i = 0; i < list->size; ++i) {
		free(list->elements[i]);
	}
	free(list->elements);
}


/* Frees only the backing store; ownership of the elements stays outside. */
void arraylist_remove(ArrayList* list) {
	free(list->elements);
}


/* Logically empties the list; existing allocations are kept for reuse. */
void arraylist_clear(ArrayList* list) {
	list->size = 0;
}
