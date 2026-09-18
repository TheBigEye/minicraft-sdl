/*
 * arraylist.h - Dynamic array of pointers (Java: java.util.ArrayList).
 *
 * This is the class the port uses everywhere the original had an ArrayList:
 * a level's entity list, the per-tile buckets, the inventory, the recipe
 * lists...
 *
 * Each list carries its own methods inside the struct, so it is used as an
 * object: `list->add(list, element)`. The constructor `arraylist_create()`
 * installs those methods, and it must run before the list is touched.
 *
 * Unlike Java, `capacity`, `size` and `elements` are public fields: Java
 * declares them private and forces a call to size(). Keeping them visible
 * is deliberate, because `for (int i = 0; i < list->size; ++i)` is idiomatic
 * C and turning size into a method would buy nothing.
 */
#ifndef UTILS_ARRAYLIST_H_
#define UTILS_ARRAYLIST_H_ 1

#include "javalang.h"

typedef struct ArrayList ArrayList;

/* Java's List, for the places that read better with the shorter name. */
typedef ArrayList List;

/* Signatures of the ArrayList methods. */
typedef void  (*arraylist_add_fn)            (ArrayList* this, void* element);
typedef void  (*arraylist_add_to_fn)         (ArrayList* this, int index, void* element);
typedef void* (*arraylist_get_fn)            (ArrayList* this, int index);
typedef void* (*arraylist_pop_fn)            (ArrayList* this);
typedef void* (*arraylist_remove_element_fn) (ArrayList* this, void* element);
typedef void* (*arraylist_remove_at_fn)      (ArrayList* this, int index);
typedef void  (*arraylist_clear_fn)          (ArrayList* this);
typedef void  (*arraylist_free_fn)           (ArrayList* this);
typedef void  (*arraylist_free_each_fn)      (ArrayList* this);

struct ArrayList {
    /* --- methods, installed by arraylist_create() --- */

    /* Appends to the end. Java: add(E) */
    arraylist_add_fn add;
    /* Inserts at `index`, shifting the tail right. Java: add(int, E) */
    arraylist_add_to_fn add_to;
    /* The element at `index`. Java: get(int) */
    arraylist_get_fn get;
    /* Removes and returns the last element. Java: remove(size() - 1) */
    arraylist_pop_fn pop;
    /* Finds `element`, removes it and returns it. Java: remove(Object) */
    arraylist_remove_element_fn remove_element;
    /* Removes the element at `index`. Java: remove(int) */
    arraylist_remove_at_fn remove_at;
    /* Empties the list but keeps the allocated storage. Java: clear() */
    arraylist_clear_fn clear;
    /* Destructor: frees the storage, not the elements. */
    arraylist_free_fn free;
    /* Destructor: frees each element and then the storage. */
    arraylist_free_each_fn free_each;

    /* --- data --- */

    /* Slots allocated in `elements`. */
    int capacity;
    /* Slots actually in use. */
    int size;
    /* The storage itself; grown with realloc() on demand. */
    void** elements;
};

/* Constructor: installs the methods and leaves the list empty. */
PUBLIC void arraylist_create(ArrayList* this);

/*
 * The method implementations. They are declared here because the
 * constructor installs them and because callers that want to skip the
 * function pointer need to see them.
 */
PUBLIC void  arraylist_add(ArrayList* this, void* element);
PUBLIC void  arraylist_add_to(ArrayList* this, int index, void* element);
PUBLIC void* arraylist_get(ArrayList* this, int index);
PUBLIC void* arraylist_pop(ArrayList* this);
PUBLIC void* arraylist_remove_element(ArrayList* this, void* element);
PUBLIC void* arraylist_remove_at(ArrayList* this, int index);
PUBLIC void  arraylist_clear(ArrayList* this);
PUBLIC void  arraylist_free(ArrayList* this);
PUBLIC void  arraylist_free_each(ArrayList* this);

#endif /* UTILS_ARRAYLIST_H_ */
