#include <stdlib.h>
#include <string.h>

#include "Array.h"

struct array
{
    size_t size;
    uint8_t sizeOfType;
    void *data;
};

array *array_create(size_t size, size_t sizeOfType)
{
    if (size == 0 || sizeOfType == 0)
    {
        return NULL;
    }
    array *arr = (array *)malloc(sizeof(array));
    if (arr == NULL)
    {
        return NULL;
    }
    arr->data = calloc(size, sizeOfType);
    if (arr->data == NULL)
    {
        free(arr);
        return NULL;
    }
    arr->size = size;
    arr->sizeOfType = sizeOfType;
    return arr;
}

void *array_at(array *array, size_t index)
{
    if (index >= array_size(array))
    {
        return NULL;
    }
    uint8_t *casted = (uint8_t *)array->data;
    uint8_t *ptr = casted + index * array->sizeOfType;
    return (void *)ptr;
}

void *array_front(array *array) { return array_at(array, 0); }

void *array_back(array *array) { return array_at(array, array_size(array) - 1); }

size_t array_size(array *array) { return array->size; }

size_t array_size_of_type(array *array) { return array->sizeOfType; }

void *array_data(array *array) { return array->data; }

void array_copy(array *source, array *destination)
{
    size_t bytes = array_size(source) * array_size_of_type(source);
    memcpy(array_data(destination), array_data(source), bytes);
}

void array_destroy(array *array)
{
    if (array == NULL)
    {
        return;
    }
    free(array->data);
    free(array);
}