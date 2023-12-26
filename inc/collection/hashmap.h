//
// Created by Bradford, Ben on 20/12/2023.
//

#ifndef HTTPSERVER_HASHMAP_H
#define HTTPSERVER_HASHMAP_H

#include <util/common.h>
#include <util/hash.h>
#include <collection/vector.h>

typedef struct sHashMap
{
    vector *bucket;
    int numBuckets;
    int size;
    HashType (*hash)(void *);
} hashmap;

bool hashmap_exists(hashmap *, HashKey key);
bool hashmap_insert(hashmap *, HashKey key, void *value);
void *hashmap_get(hashmap *, HashKey key);
bool hashmap_remove(hashmap *, HashKey key);
int hashmap_init(hashmap *, HashType (*hashFunc)(void *), int numBuckets);
void hashmap_free(hashmap *);

#endif //HTTPSERVER_HASHMAP_H
