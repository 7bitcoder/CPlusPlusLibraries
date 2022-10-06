#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct vector vector;

    vector *vector_create(size_t size, size_t sizeOfType);

    void *vector_at(vector *vector, size_t index);

    bool vector_set_at(vector *vector, size_t index, void *value);

    bool vector_get_at(vector *vector, size_t index, void *value);

    void *vector_front(vector *vector);

    bool vector_set_front(vector *vector, void *value);

    bool vector_get_front(vector *vector, void *value);

    void *vector_back(vector *vector);

    bool vector_set_back(vector *vector, void *value);

    bool vector_get_back(vector *vector, void *value);

    void *vector_data(vector *vector);

    size_t vector_size(vector *vector);

    bool vector_empty(vector *vector);

    bool vector_append(vector *vector, void *value);

    bool vector_pop(vector *vector, void *value);

    void vector_erase(vector *vector, size_t index);

    void vector_clear(vector *vector);

    void vector_destroy(vector *vector);

#ifdef __cplusplus
}
#endif