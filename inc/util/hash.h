//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef UNTITLED_HASH_H
#define UNTITLED_HASH_H

typedef void *HashKey;
typedef unsigned long HashType;

static inline HashType inthash(void* key) {
    HashType* k = (HashType*)key;
    return *k;
}

static inline HashType strhash(void *string)
{
    char *str = (char*)string;
    HashType hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

#endif //UNTITLED_HASH_H
