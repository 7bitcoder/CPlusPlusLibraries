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

    void *vector_front(vector *vector);

    void *vector_back(vector *vector);

    void *vector_data(vector *vector);

    size_t vector_size(vector *vector);

    bool vector_empty(vector *vector);

    void *vector_expand(vector *vector);

    void vector_shrink(vector *vector);

    void vector_erase(vector *vector, size_t index);

    void vector_clear(vector *vector);

    void vector_destroy(vector *vector);

#ifdef __cplusplus
}
#endif