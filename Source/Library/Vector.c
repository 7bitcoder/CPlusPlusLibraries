#include <stdint.h>
#include <stdlib.h>

#include "Array.h"
#include "Vector.h"

struct vector
{
    size_t size;
    array *array;
};

size_t get_array_size(size_t vectorSize)
{
    if (vectorSize < 5)
    {
        return 5;
    }
    return vectorSize + 10;
}

vector *vector_create(size_t size, size_t sizeOfType)
{
    vector *vec = (vector *)malloc(sizeof(vector));
    if (vec == NULL)
    {
        return NULL;
    }

    array *array = array_create(get_array_size(size), sizeOfType);
    if (array == NULL)
    {
        free(vec);
        return NULL;
    }
    vec->size = size;
    vec->array = array;
    return vec;
}

void *vector_at(vector *vector, size_t index)
{
    if (index >= vector_size(vector))
    {
        return NULL;
    }
    return array_at(vector->array, index);
}

void *vector_front(vector *vector)
{
    if (vector_empty(vector))
    {
        return NULL;
    }
    return array_front(vector->array);
}

void *vector_back(vector *vector)
{
    if (vector_empty(vector))
    {
        return NULL;
    }
    return array_at(vector->array, vector_size(vector) - 1);
}

void *vector_data(vector *vector) { return array_data(vector->array); }

size_t vector_size(vector *vector) { return vector->size; }

bool vector_empty(vector *vector) { return vector_size(vector) == 0; }

void *vector_expand(vector *vector)
{
    if (vector_size(vector) < array_size(vector->array))
    {
        vector->size++;
        return vector_back(vector);
    }
    size_t newArraySize = get_array_size(vector->size + 1);

    array *newArray = array_create(newArraySize, array_size_of_type(vector->array));
    if (newArray == NULL)
    {
        return NULL;
    }

    array_copy(vector->array, newArray);
    array_destroy(vector->array);
    vector->array = newArray;
    vector->size++;
    return vector_back(vector);
}

void vector_shrink(vector *vector)
{
    if (vector->size > 0)
    {
        vector->size--;
    }
}

void vector_erase(vector *vector, size_t index) {}

void vector_clear(vector *vector) { vector->size = 0; }

void vector_destroy(vector *vector)
{
    if (vector == NULL)
    {
        return;
    }
    array_destroy(vector->array);
    free(vector);
}
