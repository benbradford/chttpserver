//
// Created by Bradford, Ben on 20/12/2023.
//

#include <collection/hashmap.h>
#include <memory.h>

typedef struct
{
    HashKey key;
    void *value;
} HashmapEntry;

int bucketId(hashmap* map, HashKey key)
{
    return map->hash(key) % map->numBuckets;
}

vector *getBucket(hashmap* map, HashKey key)
{
    int id = bucketId(map, key);
    return &map->bucket[id];
}

bool hashmap_exists(hashmap *map, HashKey key)
{
    if (!map)
    {
        return false;
    }
    printf("Checking %s\n", (char*) key);
    vector *bucket= getBucket(map, key);
    for (int i = 0; i < bucket->size; ++i)
    {
        HashmapEntry *entry = (HashmapEntry *)vector_get(bucket, i);
        if (entry->key == key)
        {
            return true;
        }
    }
    return false;
}

bool hashmap_insert(hashmap *map, HashKey key, void *value)
{
    if (!map)
    {
        return false;
    }
    vector *bucket= getBucket(map, key);

    for (int i = 0; i < bucket->size; ++i)
    {
        HashmapEntry *entry = (HashmapEntry *)vector_get(bucket, i);
        if (entry->key == key)
        {
            entry->value = value;
            return true;
        }
    }

    ++map->size;

    HashmapEntry *v = malloc(sizeof(HashmapEntry));
    v->value = value;
    v->key = key;
    vector_pushBack(bucket, v);
    printf("Adding %s\n", (char*) key);

    return true;
}

void *hashmap_get(hashmap *map, HashKey key)
{
    if (!map)
    {
        return NULL;
    }
    vector *bucket= getBucket(map, key);

    for (int i = 0; i < bucket->size; ++i)
    {
        HashmapEntry *entry = (HashmapEntry *)vector_get(bucket, i);
        if (entry->key == key)
        {
            return entry->value;
        }
    }

    return NULL;
}

bool hashmap_remove(hashmap *map, HashKey key)
{
    if (!map)
    {
        return false;
    }

    vector *bucket = getBucket(map, key);

    for (int i = 0; i < bucket->size; ++i)
    {
        HashmapEntry *entry = (HashmapEntry *)vector_get(bucket, i);
        if (entry->key == key)
        {
            free(entry);
            vector_delete(bucket, i);
            --map->size;
            return true;
        }
    }
    return false;
}

int hashmap_init(hashmap *map, HashType (*hashFunc)(void*), int numBuckets)
{
    if (!map)
    {
        return -1;
    }
    map->hash = hashFunc;
    map->size = 0;
    map->numBuckets = numBuckets;
    map->bucket = malloc(sizeof(vector) * map->numBuckets);
    for (int i = 0; i < map->numBuckets; ++i)
    {
        int initVecRes = vector_init(&map->bucket[i], 4);
        if (initVecRes < 0)
        {
            return -1;
        }
    }
    return 0;
}

void hashmap_free(hashmap *map)
{
    if (!map)
    {
        return;
    }
    for (int i = 0; i < map->numBuckets; ++i)
    {
        vector_free(&map->bucket[i]);
    }
    free(map->bucket);
    map->bucket = NULL;
}