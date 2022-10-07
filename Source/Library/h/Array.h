#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct array array;

    array *array_create(size_t length, size_t itemSize);

    array *array_clone(array *arrayToClone);

    array *array_from_data(void *data, size_t length, size_t itemSize);

    bool array_copy(array *destination, array *source, size_t length);

    bool array_copy_data(array *destination, void *source, size_t length);

    void *array_at(array *array, size_t index);

    bool array_get_at(array *array, size_t index, void *item);

    bool array_set_at(array *array, size_t index, void *item);

    void *array_front(array *array);

    bool array_get_front(array *array, void *item);

    bool array_set_front(array *array, void *item);

    void *array_back(array *array);

    bool array_get_back(array *array, void *item);

    bool array_set_back(array *array, void *item);

    void *array_data(array *array);

    size_t array_length(array *array);

    size_t array_item_size(array *array);

    void array_destroy(array *array);

#ifdef __cplusplus
}
#endif