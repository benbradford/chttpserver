//
// Created by Bradford, Ben on 18/12/2023.
//
#include <util/stringbuilder.h>

int sb_init(stringbuilder *b, const char *s)
{
    if (!b)
    {
        return -1;
    }
    b->count = strlen(s);
    b->cap = 2 * (b->count + 1);
    b->mem = malloc(b->cap);
    if (!b->mem) {
        b->count = 0;
        b->cap = 0;
        return -1;
    }
    strcpy(b->mem, s);
    return 0;
}

int sb_appendNewLine(stringbuilder *b, const char *c)
{
    int res = sb_append(b, c);
    if (res != 0)
    {
        return res;
    }
    return sb_newLine(b);
}

int sb_newLine(stringbuilder *b)
{
    return sb_append(b, "\n");
}

int sb_append(stringbuilder *b, const char *c)
{
    if (!b)
    {
        return -1;
    }
    while (*c !='\0')
    {
        b->mem[b->count] = *c;
        ++b->count;
        if (b->count == b->cap) {
            char *new_mem = realloc(b->mem, b->cap * 2);
            if (!new_mem) {
                return -1;
            }
            memset(new_mem + b->cap, 0, b->cap);
            b->mem = new_mem;
            b->cap *= 2;
        }
        ++c;
    }
    return 0;
}

char *sb_toString(stringbuilder *b)
{
    return b->mem;
}

int sb_free(stringbuilder *b)
{
    if (!b)
    {
        return -1;
    }
    free(b->mem);
    b->cap = 0;
    b->count = 0;
    return 0;
}