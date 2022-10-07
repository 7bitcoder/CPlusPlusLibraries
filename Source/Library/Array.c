#include <stdlib.h>
#include <string.h>

#include "Array.h"

struct array
{
    size_t length;
    uint8_t itemSize;
    void *data;
};

array *array_create(size_t length, size_t itemSize)
{
    if (length == 0 || itemSize == 0)
    {
        return NULL;
    }
    array *arr = (array *)malloc(sizeof(array));
    if (arr == NULL)
    {
        return NULL;
    }
    arr->data = calloc(length, itemSize);
    if (arr->data == NULL)
    {
        free(arr);
        return NULL;
    }
    arr->length = length;
    arr->itemSize = itemSize;
    return arr;
}

array *array_clone(array *arrayToClone)
{
    array *newArray = array_create(array_length(arrayToClone), array_item_size(arrayToClone));
    if (newArray == NULL || !array_copy(newArray, arrayToClone, array_length(arrayToClone)))
    {
        return NULL;
    }
    return newArray;
}

array *array_from_data(void *data, size_t length, size_t itemSize)
{
    array *newArray = array_create(length, itemSize);
    if (newArray == NULL || !array_copy_data(newArray, data, length))
    {
        return NULL;
    }
    return newArray;
}

bool array_copy(array *destination, array *source, size_t length)
{
    size_t sourceLen = array_length(source), sourceItemSize = array_item_size(source);
    size_t destLen = array_length(destination), destItemSize = array_item_size(destination);
    if (sourceItemSize != destItemSize || length > sourceLen || sourceLen > destLen)
    {
        return false;
    }
    return array_copy_data(destination, array_data(source), length);
}

bool array_copy_data(array *destination, void *source, size_t length)
{
    memcpy(array_data(destination), source, length * array_item_size(destination));
    return true;
}

void *array_at(array *array, size_t index)
{
    if (index >= array_length(array))
    {
        return NULL;
    }
    uint8_t *casted = (uint8_t *)array_data(array);
    uint8_t *ptr = casted + index * array_item_size(array);
    return (void *)ptr;
}

bool array_get_at(array *array, size_t index, void *item)
{
    void *ptr = array_at(array, index);
    if (ptr == NULL || item == NULL)
    {
        return false;
    }
    memcpy(item, ptr, array_item_size(array));
    return true;
}

bool array_set_at(array *array, size_t index, void *item)
{
    void *ptr = array_at(array, index);
    if (ptr == NULL || item == NULL)
    {
        return false;
    }
    memcpy(ptr, item, array_item_size(array));
    return true;
}

void *array_front(array *array) { return array_at(array, 0); }

bool array_get_front(array *array, void *item) { return array_get_at(array, 0, item); }

bool array_set_front(array *array, void *item) { return array_set_at(array, 0, item); }

void *array_back(array *array) { return array_at(array, array_length(array) - 1); }

bool array_get_back(array *array, void *item) { return array_get_at(array, array_length(array) - 1, item); }

bool array_set_back(array *array, void *item) { return array_set_at(array, array_length(array) - 1, item); }

void *array_data(array *array) { return array->data; }

size_t array_length(array *array) { return array->length; }

size_t array_item_size(array *array) { return array->itemSize; }

void array_destroy(array *array)
{
    if (array == NULL)
    {
        return;
    }
    free(array_data(array));
    free(array);
}