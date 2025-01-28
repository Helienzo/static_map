/**
 * @file:       static_map.h
 * @author:     Lucas Wennerholm <lucas.wennerholm@gmail.com>
 * @brief:      Header file for the static map
 *
 * @license: MIT License
 *
 * Copyright (c) 2025 Lucas Wennerholm
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

/**
 * To use the map do the following:
 * Create an item struct that contains both the
 * staticMapItem_t and your custom data struct
 *
 */

#ifndef STATIC_MAP_H
#define STATIC_MAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, member)	(type *)((char *)(ptr) - offsetof(type,member))
#endif

typedef enum {
    STATIC_MAP_SUCCESS,
    STATIC_MAP_NULL_ERROR   = -201,
    STATIC_MAP_EMPTY        = -202,
    STATIC_MAP_FULL         = -203,
    STATIC_MAP_UNUSED_ERASE = -204,
    STATIC_MAP_INVALID_KEY  = -205,
} staticMapErr_t;

typedef enum {
    STATIC_MAP_SLOT_EMPTY = 0, // never occupied
    STATIC_MAP_SLOT_IN_USE,    // currently in use
    STATIC_MAP_SLOT_DELETED    // was used, then removed (tombstone)
} staticMapslotState_t; 

typedef enum {
    STATIC_MAP_CB_NEXT = 0, // Keep iterating
    STATIC_MAP_CB_STOP,     // Stop iterating
    STATIC_MAP_CB_ERASE,    // Erase this node and keep iterating
} staticMapCbDo_t;

typedef struct staticMapItem staticMapItem_t;

/**
 * This item should be embedded into what ever struct that should be put in the map
 */
struct staticMapItem {
    staticMapslotState_t state;
    uint32_t             key; // This is the map key
    staticMapItem_t     *next;
    staticMapItem_t     *prev;
};

/**
 * This is the actuall map object
 */
typedef struct staticMap {
    staticMapItem_t **items;  // Pointer to an array of map item pointers
    size_t            length; // The size of the array
    staticMapItem_t  *tail;
    staticMapItem_t  *head;
} staticMap_t;

/**
 * Initialize the static map, and populate map array
 * Input: Pointer to a static map instance
 * Input: Pointer to the static map items array
 * Input: Size of the map array
 * Input: Size of each item
 * Input: Pointer to hte first item
 * Returns: staticMapErr_t
 */
int32_t staticMapInit(staticMap_t *map, staticMapItem_t **itemsArray, size_t length, size_t item_size, staticMapItem_t *first_item);

/**
 * Get the a new item at the key position, and set it as in use
 * Input: Pointer to a static map instance
 * Input: Key to new item
 * Returns: staticMapErr_t
 */
 staticMapItem_t *staticMapInsertAndGet(staticMap_t *map, uint32_t key);

/**
 * Find an item given the key
 * Input: Pointer to a static map instance
 * Input: The item key
 * Returns: An available item or NULL if no item was found
 */
staticMapItem_t* staticMapFind(staticMap_t *map, uint32_t key);

/**
 * Remove the item from the map given the actuall item
 * Input: Pointer to a static map instance
 * Input: Item to remove
 * Returns: staticMapErr_t
 */
int32_t staticMapRemove(staticMap_t *map, staticMapItem_t *item);

/**
 * Remove the item from the map given the key
 * Input: Pointer to a static map instance
 * Input: key of item
 * Returns: staticMapErr_t
 */
int32_t staticMapRemoveByKey(staticMap_t *map, uint32_t key);

/**
 * Loop throug all item in map and call the callback on each
 * Input: Pointer to a static map instance
 * Input: Callback function
 * Returns: staticMapErr_t
 */
int32_t staticMapForEach(staticMap_t *map, int32_t (*callback)(staticMap_t *map, staticMapItem_t *item));

/**
 * This is a macro that makes it more safe to initialize a static map
 */
#define STATIC_MAP_INIT(map, array, length, list) \
    staticMapInit(&(map), &((array)[0]), (length), sizeof((list)[0]), &list->node);

#endif /* STATIC_MAP_H */