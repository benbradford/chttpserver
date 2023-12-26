//
// Created by Bradford, Ben on 18/12/2023.
//
#include <collection/vector.h>

bool vector_resize(vector *v, int capacity)
{
    if(!v)
    {
        return false;
    }
    VectorData *items = realloc(v->items, sizeof(VectorData) * capacity);
    if (!items)
    {
        return false;
    }
    v->items = items;
    v->capacity = capacity;
    return true;
}

bool vector_pushBack(vector *v, VectorData item)
{
    if(!v)
    {
        return false;
    }
    if (v->capacity == v->size)
    {
        bool status = vector_resize(v, v->capacity * 2);
        if(status)
        {
            v->items[v->size++] = item;
        }
        return status;
    }
    v->items[v->size++] = item;
    return true;
}

bool vector_addAll(vector *v, const vector *from)
{
    if (!v)
    {
        return false;
    }
    bool status = true;
    for (int i = 0; i < from->size; ++i)
    {
        status &= vector_pushBack(v, vector_get(from, i));
    }
    return status;
}

bool vector_set(vector *v, VectorIndex index, VectorData item)
{
    if (v && (index >= 0) && (index < v->size))
    {
        v->items[index] = item;
        return true;
    }

    return false;
}

VectorData vector_get(const vector *v, VectorIndex index)
{
    if(!v)
    {
        return NULL;
    }
    if ((index >= 0) && (index < v->size))
    {
        return v->items[index];
    }
    return NULL;
}

VectorData vector_delete(vector *v, VectorIndex index)
{
    if(!v)
    {
        return NULL;
    }
    if ((index < 0) || (index >= v->size))
    {
        return NULL;
    }
    VectorData item = v->items[index];
    v->items[index] = NULL;
    for (int i = index; (i < v->size - 1); ++i)
    {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }
    v->size--;
    if ((v->size > 0) && ((v->size) == (v->capacity / 4)))
    {
        vector_resize(v, v->capacity / 2);
    }
    return item;
}

int vector_init(vector *v, int startCapacity)
{
    v->capacity = startCapacity;
    v->size = 0;
    v->items = calloc(v->capacity, sizeof(VectorData));
    if (!v->items)
    {
        return -1;
    }
    return 0;
}

void vector_free(vector *v)
{
    if(!v)
    {
        return;
    }
    free(v->items);
    v->items = NULL;
}
