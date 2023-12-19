//
// Created by Bradford, Ben on 19/12/2023.
//

#include <util/kvpair.h>

kvpair *kvpair_find(const kvpairs *vector, const char *name)
{
    for (int i = 0; i < vector->size; ++i) {
        kvpair *pair = vector_get(vector, i);
        if (strcmp(pair->name, name) == 0)
        {
            return pair;
        }
    }
    return NULL;
}
