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

    void *array_front(array *array);

    void *array_back(array *array);

    void *array_data(array *array);

    void array_copy(array *source, array *destination);

    size_t array_size(array *array);

    size_t array_size_of_type(array *array);

    void array_destroy(array *array);

#ifdef __cplusplus
}
#endif