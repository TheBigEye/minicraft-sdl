/*
 * arraylist.c - Implementation of ArrayList.
 *
 * Growth is by hand (realloc on demand) and element order is preserved
 * across insertions and removals, exactly as in Java's ArrayList. The game
 * logic relies on that: an entity added halfway through a tick is visited
 * on the next one, and never skipped.
 */
#include "arraylist.h"

#include <stdlib.h>

#include "../log.h"

/* Constructor: installs the methods and allocates nothing yet. */
PUBLIC void arraylist_create(ArrayList* this) {
    this->add            = arraylist_add;
    this->add_to         = arraylist_add_to;
    this->get            = arraylist_get;
    this->pop            = arraylist_pop;
    this->remove_element = arraylist_remove_element;
    this->remove_at      = arraylist_remove_at;
    this->clear          = arraylist_clear;
    this->free           = arraylist_free;
    this->free_each      = arraylist_free_each;

    this->capacity = 0;
    this->size     = 0;
    this->elements = null;
}


/*
 * Inserts `element` at `index`, shifting the tail one slot to the right.
 * Inserting at the end degenerates into add(); an index past the tail is
 * rejected, because honouring it would leave a meaningless gap.
 */
PUBLIC void arraylist_add_to(ArrayList* this, int index, void* element) {
    if (index > this->size) {
        LOG_WARN("Tried pushing element too far! (index %d, size %d)", index, this->size);
        return;

    } else if (index == this->size) {
        this->add(this, element);

    } else {
        int size = this->size + 1;

        if (size > this->capacity) {
            this->elements = realloc(this->elements, sizeof(void*) * size);
            this->capacity = size;
        }

        for (int i = this->size; i > index; --i) {
            this->elements[i] = this->elements[i - 1];
        }

        this->elements[index] = element;
        this->size = size;
    }
}


/* Appends `element`, allocating or growing the storage when it must. */
PUBLIC void arraylist_add(ArrayList* this, void* element) {
    int size = this->size + 1;

    if (size > this->capacity) {
        if (this->elements) {
            this->elements = realloc(this->elements, sizeof(void*) * size);

        } else {
            this->elements = malloc(sizeof(void*) * size);
        }

        this->capacity = size;
    }

    this->size = size;
    this->elements[this->size - 1] = element;
}


/* The element at `index`; staying in range is the caller's job. */
PUBLIC void* arraylist_get(ArrayList* this, int index) {
    return this->elements[index];
}


/* Finds the first slot holding `element`, removes it, and warns if absent. */
PUBLIC void* arraylist_remove_element(ArrayList* this, void* element) {
    for (int i = 0; i < this->size; ++i) {
        if (this->elements[i] == element) {
            return this->remove_at(this, i);
        }
    }

    LOG_WARN("Tried removing unknown element from array (%p)!", element);
    return null;
}


/* Removes the slot at `index`, closing the gap by shifting the tail left. */
PUBLIC void* arraylist_remove_at(ArrayList* this, int index) {
    void* element = this->elements[index];

    if (index < this->size - 1) {
        for (int i = index + 1; i < this->size; ++i) {
            this->elements[i - 1] = this->elements[i];
        }
    }

    --this->size;
    return element;
}


/* Removes and returns the last element; the storage keeps its capacity. */
PUBLIC void* arraylist_pop(ArrayList* this) {
    --this->size;

    return this->elements[this->size];
}


/* Frees every element and then the storage. The list must not be reused. */
PUBLIC void arraylist_free_each(ArrayList* this) {
    for (int i = 0; i < this->size; ++i) {
        free(this->elements[i]);
    }

    free(this->elements);
}


/* Frees the storage only; somebody else owns the elements. */
PUBLIC void arraylist_free(ArrayList* this) {
    free(this->elements);
}


/* Empties the list logically, keeping the storage around for reuse. */
PUBLIC void arraylist_clear(ArrayList* this) {
    this->size = 0;
}
