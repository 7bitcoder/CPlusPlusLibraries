#include <stdint.h>
#include <stdlib.h>

#include "Vector.h"

struct vector
{
    size_t length;
    array *array;
};

// private methods
array *vector_array(vector *vector);

size_t get_computed_array_length(size_t vectorSize)
{
    if (vectorSize < 5)
    {
        return 5;
    }
    return vectorSize + 10;
}

// public implementation
vector *vector_create(size_t length, size_t itemSize)
{
    vector *vec = (vector *)malloc(sizeof(vector));
    if (vec == NULL)
    {
        return NULL;
    }

    array *array = array_create(get_computed_array_length(length), itemSize);
    if (array == NULL)
    {
        free(vec);
        return NULL;
    }
    vec->length = length;
    vec->array = array;
    return vec;
}

vector *vector_clone(vector *vectorToClone)
{
    vector *vec = vector_create(vector_length(vectorToClone), vector_item_size(vectorToClone));
    if (vec == NULL || !vector_copy(vec, vectorToClone, vector_length(vectorToClone)))
    {
        return NULL;
    }
    vec->length = vector_length(vectorToClone);
    return vec;
}

vector *vector_from_array(array *array)
{
    vector *vec = vector_create(array_length(array), array_item_size(array));
    if (vec == NULL || !vector_copy_array(vec, array, array_length(array)))
    {
        return NULL;
    }
    return vec;
}

vector *vector_from_data(void *data, size_t length, size_t itemSize)
{
    vector *vec = vector_create(length, itemSize);
    if (vec == NULL || !vector_copy_data(vec, data, length))
    {
        return NULL;
    }
    return vec;
}

bool vector_copy(vector *destination, vector *source, size_t length)
{
    size_t sourceLen = vector_length(source), sourceItemSize = vector_item_size(source);
    size_t destLen = vector_length(destination), destItemSize = vector_item_size(destination);
    if (sourceItemSize != destItemSize || length > sourceLen || sourceLen > destLen)
    {
        return false;
    }

    return vector_copy_array(destination, vector_array(source), length);
}

bool vector_copy_array(vector *destination, array *source, size_t length)
{
    return array_copy(vector_array(destination), source, length);
}

bool vector_copy_data(vector *destination, void *source, size_t length)
{
    return array_copy_data(vector_array(destination), source, length);
}

void *vector_at(vector *vector, size_t index)
{
    if (index >= vector_length(vector))
    {
        return NULL;
    }
    return array_at(vector->array, index);
}

bool vector_get_at(vector *vector, size_t index, void *value)
{
    if (index >= vector_length(vector))
    {
        return false;
    }
    return array_get_at(vector->array, index, value);
}

bool vector_set_at(vector *vector, size_t index, void *value)
{
    if (index >= vector_length(vector))
    {
        return false;
    }
    return array_set_at(vector->array, index, value);
}

void *vector_front(vector *vector) { return vector_at(vector, 0); }

bool vector_get_front(vector *vector, void *value) { return vector_get_at(vector, 0, value); }

bool vector_set_front(vector *vector, void *value) { return vector_set_at(vector, 0, value); }

void *vector_back(vector *vector) { return vector_at(vector, vector_length(vector) - 1); }

bool vector_get_back(vector *vector, void *value) { return vector_get_at(vector, vector_length(vector) - 1, value); }

bool vector_set_back(vector *vector, void *value) { return vector_set_at(vector, vector_length(vector) - 1, value); }

void *vector_data(vector *vector) { return array_data(vector->array); }

size_t vector_length(vector *vector) { return vector->length; }

size_t vector_capacity(vector *vector) { return array_length(vector->array); }

size_t vector_item_size(vector *vector) { return array_item_size(vector->array); }

array *vector_array(vector *vector) { return vector->array; }

bool vector_empty(vector *vector) { return vector_length(vector) == 0; }

bool vector_append(vector *vector, void *value)
{
    size_t currentLen = vector_length(vector);
    if (currentLen < vector_capacity(vector))
    {
        vector->length++;
        return vector_set_back(vector, value);
    }
    size_t newArraySize = get_computed_array_length(currentLen + 1);

    array *newArray = array_create(newArraySize, vector_item_size(vector));
    if (newArray == NULL)
    {
        return false;
    }

    array *oldArray = vector_array(vector);
    vector->array = newArray;
    vector->length++;
    vector_copy_array(vector, oldArray, currentLen);
    array_destroy(oldArray);
    return vector_set_back(vector, value);
}

bool vector_pop(vector *vector, void *value)
{
    bool result = vector_get_back(vector, value);
    if (result && vector->length > 0)
    {
        vector->length--;
    }
    return result;
}

void vector_erase(vector *vector, size_t index) {}

void vector_clear(vector *vector) { vector->length = 0; }

void vector_destroy(vector *vector)
{
    if (vector == NULL)
    {
        return;
    }
    array_destroy(vector->array);
    free(vector);
}
