#ifndef UTILS_ARRAYLIST_H_
#define UTILS_ARRAYLIST_H_ 1

typedef struct{
	int capacity;
	int size;
	void** elements;
} ArrayList;

void create_arraylist(ArrayList* list);
void arraylist_pushTo(ArrayList* list, int index, void* element);
void arraylist_push(ArrayList* list, void* element);
void* arraylist_pop(ArrayList* list);
void* arraylist_removeElement(ArrayList* list, void* element);
void* arraylist_removeId(ArrayList* list, int index);
void* arraylist_get(ArrayList* list, int index);
void arraylist_remove_and_dealloc_each(ArrayList* list);
void arraylist_remove(ArrayList* list);
void arraylist_clear(ArrayList* list);

#endif /* UTILS_ARRAYLIST_H_ */
