#ifndef H_D_ARRAY
#define H_D_ARRAY

#include <stdlib.h>
#include "utils/logger.h"

typedef struct d_array {
    int* data;
    size_t size;
    size_t capacity;
} d_array;

d_array d_array_init(size_t capacity);
void d_array_push(d_array* array, int value);
int d_array_get(d_array* array, int index);
void d_array_set(d_array* array, size_t index, int value);
void d_array_reset(d_array* array);
void d_array_free(d_array* array);

d_array d_array_init(size_t capacity) {
    d_array arr;
    arr.data = (int*)malloc(sizeof(int) * capacity);
    arr.size = 0;
    arr.capacity = capacity;
    return arr;
}

void d_array_push(d_array* arr, int value) {
    if(arr->size++ > arr->capacity) {
        arr->capacity *= 2;
        arr->data = (int*)realloc(arr->data, sizeof(int) * arr->capacity);
    }
    arr->data[arr->size] = value;
}

int d_array_get(d_array* array, int index) {
    if(array->size > index) {
        LOG_ERROR("Array out of bound\n");
        return 0;
    }
    return array->data[index];
}

void d_array_set(d_array* arr, size_t index, int value) {
    if(arr->size > index) {
        LOG_ERROR("Array out of bound %s\n");
        return;
    }

    arr->data[index] = value;
}

inline void d_array_reset(d_array* arr) {
    arr->size = 0;
}

void d_array_free(d_array* arr) {
    if(arr->data) free(arr->data);
}

#endif