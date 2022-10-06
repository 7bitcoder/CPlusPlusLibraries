#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct array array;

    array *array_create(size_t size, size_t sizeOfType);

    void *array_at(array *array, size_t index);

    bool array_set_at(array *array, size_t index, void *value);

    bool array_get_at(array *array, size_t index, void *value);

    void *array_front(array *array);

    bool array_set_front(array *array, void *value);

    bool array_get_front(array *array, void *value);

    void *array_back(array *array);

    bool array_set_back(array *array, void *value);

    bool array_get_back(array *array, void *value);

    void *array_data(array *array);

    void array_copy(array *source, array *destination);

    size_t array_size(array *array);

    size_t array_size_of_type(array *array);

    void array_destroy(array *array);

#ifdef __cplusplus
}
#endif