#include "arraylist.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void create_arraylist(ArrayList* list){
	list->capacity = 0;
	list->size = 0;
	list->elements = 0;
}


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


void* arraylist_get(ArrayList* list, int index) {
	return list->elements[index];
}


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


void* arraylist_pop(ArrayList* list) {
	--list->size;
	void* elem = list->elements[list->size];
	return elem;
}


void arraylist_remove_and_dealloc_each(ArrayList* list) {
	for (int i = 0; i < list->size; ++i) {
		free(list->elements[i]);
	}
	free(list->elements);
}


void arraylist_remove(ArrayList* list) {
	free(list->elements);
}


void arraylist_clear(ArrayList* list) {
	list->size = 0;
}
