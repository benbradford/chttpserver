//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_VECTOR_H
#define HTTPSERVER_VECTOR_H

#include <util/common.h>

typedef int VectorIndex;
typedef void *VectorData;

typedef struct sVector
{
    VectorData *items;
    int capacity;
    int size;
} vector;

bool vector_resize(vector *, int capacity);
bool vector_pushBack(vector *, VectorData item);
bool vector_set(vector *, VectorIndex index, VectorData item);
VectorData vector_get(const vector *, VectorIndex index);
VectorData vector_delete(vector *, VectorIndex index);
int vector_init(vector *, int startCapacity);
int vector_free(vector *);

#endif //HTTPSERVER_VECTOR_H
