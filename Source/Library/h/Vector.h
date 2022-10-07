#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "Array.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct vector vector;

    vector *vector_create(size_t length, size_t itemSize);

    vector *vector_clone(vector *vectorToClone);

    vector *vector_from_array(array *array);

    vector *vector_from_data(void *data, size_t length, size_t itemSize);

    bool vector_copy(vector *destination, vector *source, size_t length);

    bool vector_copy_array(vector *destination, array *source, size_t length);

    bool vector_copy_data(vector *destination, void *source, size_t length);

    void *vector_at(vector *vector, size_t index);

    bool vector_get_at(vector *vector, size_t index, void *item);

    bool vector_set_at(vector *vector, size_t index, void *item);

    void *vector_front(vector *vector);

    bool vector_get_front(vector *vector, void *item);

    bool vector_set_front(vector *vector, void *item);

    void *vector_back(vector *vector);

    bool vector_get_back(vector *vector, void *item);

    bool vector_set_back(vector *vector, void *item);

    void *vector_data(vector *vector);

    size_t vector_length(vector *vector);

    size_t vector_capacity(vector *vector);

    size_t vector_item_size(vector *vector);

    bool vector_empty(vector *vector);

    bool vector_append(vector *vector, void *item);

    bool vector_pop(vector *vector, void *item);

    void vector_erase(vector *vector, size_t index);

    void vector_clear(vector *vector);

    void vector_destroy(vector *vector);

#ifdef __cplusplus
}
#endif