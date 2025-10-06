#include "static_map.h"
#include <stdio.h>

#define NUM_ITEMS_IN_MAP 10
#define FIRST_ITEM 25
#define SECOND_ITEM 15
#define THIRD_ITEM 32

typedef struct {
    uint32_t data;
    staticMapItem_t node;
} myItem_t;

staticMap_t my_map = {0};
staticMapItem_t * map_array[NUM_ITEMS_IN_MAP];
myItem_t my_item_map[NUM_ITEMS_IN_MAP];

static myItem_t * insertDataItem(staticMap_t *map, uint32_t data, uint32_t key) {
    staticMapItem_t * map_item = staticMapInsertAndGet(map, key);
    if (map_item == NULL) {
        return NULL;
    }

    myItem_t * my_item = CONTAINER_OF(map_item, myItem_t, node);

    my_item->data = data;

    return my_item;
}

static myItem_t * findItem(staticMap_t *map, uint32_t key) {
    staticMapItem_t * map_item = staticMapFind(map, key);
    if (map_item == NULL) {
        return NULL;
    }

    myItem_t * my_item = CONTAINER_OF(map_item, myItem_t, node);

    return my_item;
}

static uint32_t getDataFromItem(staticMap_t *map, uint32_t *data, uint32_t key) {
    // Find an item
    staticMapItem_t * map_item = staticMapFind(map, key);

    if (map_item == NULL) {
        return STATIC_MAP_INVALID_KEY;
    }

    myItem_t * my_item = CONTAINER_OF(map_item, myItem_t, node);

    *data = my_item->data;

    return STATIC_MAP_SUCCESS;
}

static uint32_t removeItemByKey(staticMap_t *map, uint32_t key) {
    int32_t result = staticMapRemoveByKey(map, key);
    return result;
}

static uint32_t removeItem(staticMap_t *map, myItem_t *item) {
    int32_t result = staticMapRemove(map, &item->node);
    return result;
}

static int32_t mapItemCb(staticMap_t *map, staticMapItem_t *map_item) {
    myItem_t * my_item = CONTAINER_OF(map_item, myItem_t, node);
    printf("Static Map for each item data %u\n", my_item->data);
    return STATIC_MAP_CB_NEXT;
}

static int32_t mapItemEraseCb(staticMap_t *map, staticMapItem_t *map_item) {
    myItem_t * my_item = CONTAINER_OF(map_item, myItem_t, node);
    printf("Static Map for each erase item data %u\n", my_item->data);
    if (my_item->node.key == FIRST_ITEM) {
        return STATIC_MAP_CB_ERASE;
    }
    return STATIC_MAP_CB_NEXT;
}

int main(void) {
    int32_t result = STATIC_MAP_INIT(my_map, map_array, NUM_ITEMS_IN_MAP, my_item_map);
    printf("Static Map inti result %i\n", result);

    // Test: map should be empty after init
    int32_t num_items = staticMapGetNumItems(&my_map);
    if (num_items != 0) {
        printf("Test failed: Map should be empty after init, got %i items\n", num_items);
        return 1;
    }
    printf("Test passed: Map is empty after init (0 items)\n");

    // Add a first item
    myItem_t * new_item = insertDataItem(&my_map, 100, SECOND_ITEM);

    if (new_item == NULL) {
        printf("Static insert failed!\n");
        return 1;
    }

    printf("Static Map new item key %u\n", new_item->node.key);
    printf("Static Map new item data %u\n", new_item->data);

    // Test: map should have 1 item
    num_items = staticMapGetNumItems(&my_map);
    if (num_items != 1) {
        printf("Test failed: Map should have 1 item, got %i items\n", num_items);
        return 1;
    }
    printf("Test passed: Map has 1 item\n");

    // Add a second item
    new_item = insertDataItem(&my_map, 110, FIRST_ITEM);

    if (new_item == NULL) {
        printf("Static insert failed!\n");
        return 1;
    }

    printf("Static Map new item key %u\n", new_item->node.key);
    printf("Static Map new item data %u\n", new_item->data);

    // Add a third item
    new_item = insertDataItem(&my_map, 1337, THIRD_ITEM);

    if (new_item == NULL) {
        printf("Static insert failed!\n");
        return 1;
    }

    printf("Static Map new item key %u\n", new_item->node.key);
    printf("Static Map new item data %u\n", new_item->data);

    // Test: map should have 3 items
    num_items = staticMapGetNumItems(&my_map);
    if (num_items != 3) {
        printf("Test failed: Map should have 3 items, got %i items\n", num_items);
        return 1;
    }
    printf("Test passed: Map has 3 items\n");

    // Find an item
    myItem_t * find_item = findItem(&my_map, FIRST_ITEM);

    if (find_item == NULL) {
        printf("Static find failed!\n");
        return 1;
    }

    printf("Static Map new item key %u\n", find_item->node.key);
    printf("Static Map new item data %u\n", find_item->data);

    // Find an item
    uint32_t data = 0;
    result = getDataFromItem(&my_map, &data, FIRST_ITEM);

    if (result != STATIC_MAP_SUCCESS) {
        printf("Static get data failed!\n");
        return 1;
    }

    printf("Static Map data %u\n", data);

    result = staticMapForEach(&my_map, mapItemCb);
    if (result != STATIC_MAP_SUCCESS) {
        printf("For Each failed!\n");
        return 1;
    }

    // Remove an item item
    result = removeItemByKey(&my_map, FIRST_ITEM);

    if (result != STATIC_MAP_SUCCESS) {
        printf("Static remove failed!\n");
        return 1;
    }
    printf("Erase of item %u success\n", FIRST_ITEM);

    // Test: map should have 2 items after removing one
    num_items = staticMapGetNumItems(&my_map);
    if (num_items != 2) {
        printf("Test failed: Map should have 2 items after removal, got %i items\n", num_items);
        return 1;
    }
    printf("Test passed: Map has 2 items after removal\n");

    // Find an item
    myItem_t * find_erased_item = findItem(&my_map, FIRST_ITEM);

    if (find_erased_item != NULL) {
        printf("Static find erased item failed!\n");
        return 1;
    }

    result = staticMapForEach(&my_map, mapItemCb);
    if (result != STATIC_MAP_SUCCESS) {
        printf("For Each failed!\n");
        return 1;
    }

    // Find the second item
    find_item = findItem(&my_map, SECOND_ITEM);

    if (find_item == NULL) {
        printf("Static find failed!\n");
        return 1;
    }

    printf("Static Map new item key %u\n", find_item->node.key);
    printf("Static Map new item data %u\n", find_item->data);

    // Remove an item item
    result = removeItem(&my_map, find_item);

    if (result != STATIC_MAP_SUCCESS) {
        printf("Static remove failed!\n");
        return 1;
    }

    // Find an item
    find_erased_item = findItem(&my_map, SECOND_ITEM);

    if (find_erased_item != NULL) {
        printf("Static find erased item failed!\n");
        return 1;
    }
    printf("Erase of item %u success\n", SECOND_ITEM);

    result = staticMapForEach(&my_map, mapItemCb);
    if (result != STATIC_MAP_SUCCESS) {
        printf("For Each failed!\n");
        return 1;
    }

    // Remove an item item
    result = removeItemByKey(&my_map, THIRD_ITEM);

    if (result != STATIC_MAP_SUCCESS) {
        printf("Static remove failed!\n");
        return 1;
    }
    printf("Erase of item %u success\n", FIRST_ITEM);

    result = staticMapForEach(&my_map, mapItemCb);
    if (result != STATIC_MAP_SUCCESS) {
        printf("For Each failed!\n");
        return 1;
    }

    // Add a first item
    new_item = insertDataItem(&my_map, 100, SECOND_ITEM);

    if (new_item == NULL) {
        printf("Static insert failed!\n");
        return 1;
    }

    printf("Static Map new item key %u\n", new_item->node.key);
    printf("Static Map new item data %u\n", new_item->data);

    // Add a first item
    new_item = insertDataItem(&my_map, 57, FIRST_ITEM);

    if (new_item == NULL) {
        printf("Static insert failed!\n");
        return 1;
    }

    printf("Static Map new item key %u\n", new_item->node.key);
    printf("Static Map new item data %u\n", new_item->data);


    result = staticMapForEach(&my_map, mapItemEraseCb);
    if (result != STATIC_MAP_SUCCESS) {
        printf("For Each failed!\n");
        return 1;
    }

    result = staticMapForEach(&my_map, mapItemEraseCb);
    if (result != STATIC_MAP_SUCCESS) {
        printf("For Each failed!\n");
        return 1;
    }

    // Test: map should have 1 item remaining
    num_items = staticMapGetNumItems(&my_map);
    if (num_items != 1) {
        printf("Test failed: Map should have 1 item remaining, got %i items\n", num_items);
        return 1;
    }
    printf("Test passed: Map has 1 item remaining\n");

    // Remove an item item
    result = removeItemByKey(&my_map, SECOND_ITEM);

    if (result != STATIC_MAP_SUCCESS) {
        printf("Static remove failed!\n");
        return 1;
    }
    printf("Erase of item %u success\n", SECOND_ITEM);

    // Test: map should have 0 item remaining
    num_items = staticMapGetNumItems(&my_map);
    if (num_items != 0) {
        printf("Test failed: Map should have 0 item remaining, got %i items\n", num_items);
        return 1;
    }
    printf("Test passed: Map has 0 item remaining\n");

    printf("\nAll tests passed!\n");
    return result;
}