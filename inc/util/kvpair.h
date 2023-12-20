//
// Created by Bradford, Ben on 19/12/2023.
//

#ifndef HTTPSERVER_KVPAIR_H
#define HTTPSERVER_KVPAIR_H

#include "collection/vector.h"

typedef struct sKvPair
{
    const char* name;
    const char* value;
} kvpair;

typedef struct sVector kvpairs;

kvpair *kvpair_find(const kvpairs*, const char *name);
int kvpair_freeAll(kvpairs*);

#endif //HTTPSERVER_KVPAIR_H
