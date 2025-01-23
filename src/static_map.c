/**
 * @file:       static_map.c
 * @author:     Lucas Wennerholm <lucas.wennerholm@gmail.com>
 * @brief:      Implementation of the static map
 *
 * @license: MIT License
 *
 * Copyright (c) 2024 Lucas Wennerholm
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "static_map.h"

// For linear probing, once we collide at index = hash(key),
// we'll do: index = (index + 1) % length
#define LINEAR_PROBE(i, length)  (((i) + 1) % (length))

// A simple hash for an 32-bit key. 
// Could be improved with more sofisticated algo if needed
static inline uint32_t hash_func(staticMap_t *map, uint32_t key) {
    return key % map->length;
}

int32_t staticMapInit(staticMap_t *map, staticMapItem_t **itemsArray, size_t length, size_t item_size, staticMapItem_t *first_item) {
    if (map == NULL || itemsArray == NULL || length == 0 || item_size < sizeof(staticMapItem_t) || first_item == NULL) {
        return STATIC_MAP_NULL_ERROR;
    }

    map->items            = itemsArray;   // The user-provided array
    map->length           = length;
    map->num_active_items = 0;

    staticMapItem_t * item = first_item;
    for (uint32_t i = 0; i < length; i++) {
        map->items[i] = item;
        item->key     = 0;
        item->state   = STATIC_MAP_SLOT_EMPTY;
        item = (staticMapItem_t*)((uint8_t*)item + item_size);
    }

    return STATIC_MAP_SUCCESS;
}

staticMapItem_t *staticMapInsertAndGet(staticMap_t *map, uint32_t key) {
    if (map == NULL) {
        return NULL;
    }

    // Calculate initial bucket
    uint32_t index = hash_func(map, key);

    for (uint32_t attempt = 0; attempt < map->length; attempt++) {
        staticMapItem_t *slot = map->items[index];

        if (slot->state == STATIC_MAP_SLOT_EMPTY || slot->state == STATIC_MAP_SLOT_DELETED) {
            // Found an empty or tombstoned slot, use it
            slot->state  = STATIC_MAP_SLOT_IN_USE;
            slot->key = key;
            map->num_active_items++;
            return slot;
        }
        else if (slot->state == STATIC_MAP_SLOT_IN_USE && slot->key == key) {
            // The key is not unique, that is not a valid use case
            return NULL;
        }

        // Collision: probe the next slot
        index = LINEAR_PROBE(index, map->length);
    }

    // Map is full
    return NULL;
}

staticMapItem_t* staticMapFind(staticMap_t *map, uint32_t key) {
    if (map == NULL) {
        return NULL;
    }

    uint32_t index = hash_func(map, key);

    for (uint32_t attempt = 0; attempt < map->length; attempt++) {
        staticMapItem_t *slot = map->items[index];

        if (slot->state == STATIC_MAP_SLOT_EMPTY) {
            // We hit an empty slot, means the key is not in the table
            return NULL;
        }
        else if (slot->state == STATIC_MAP_SLOT_IN_USE && slot->key == key) {
            // Found it
            return slot;
        }
        // else tombstone or different key => keep probing
        index = LINEAR_PROBE(index, map->length);
    }

    return NULL; // Not found
}

int32_t staticMapRemove(staticMap_t *map, staticMapItem_t *item) {
    if (map == NULL || item == NULL) {
        return STATIC_MAP_NULL_ERROR;
    }

    // Check if this item is in use
    if (item->state != STATIC_MAP_SLOT_IN_USE) {
        // This is not good, it means that a pointer to an item is used after remove
        // this is similar to use after free, or double free
        return STATIC_MAP_UNUSED_ERASE;
    }

    item->state = STATIC_MAP_SLOT_DELETED;

    map->num_active_items--;

    // TODO, we could add some kind of cleanup routine here to get rid of tombstones

    return STATIC_MAP_SUCCESS;
}

int32_t staticMapRemoveByKey(staticMap_t *map, uint32_t key) {
    if (map == NULL) {
        return STATIC_MAP_NULL_ERROR;
    }

    uint32_t index = hash_func(map, key);

    for (uint32_t attempt = 0; attempt < map->length; attempt++) {
        staticMapItem_t *slot = map->items[index];

        if (slot->state == STATIC_MAP_SLOT_EMPTY) {
            // We reached an empty slot; the key isn't in the table
            return STATIC_MAP_UNUSED_ERASE;
        }

        if (slot->state == STATIC_MAP_SLOT_IN_USE && slot->key == key) {
            // Found the key; mark this slot as deleted
            slot->state = STATIC_MAP_SLOT_DELETED;
            map->num_active_items--;
            return STATIC_MAP_SUCCESS;
        }
        index = LINEAR_PROBE(index, map->length);
    }

    return STATIC_MAP_INVALID_KEY;
}
