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
    b->size = strlen(s);
    b->capacity = 2 * (b->size + 1);
    b->buffer = calloc(b->capacity, sizeof(char));
    if (!b->buffer) {
        b->size = 0;
        b->capacity = 0;
        return -1;
    }
    strcpy(b->buffer, s);
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
        b->buffer[b->size] = *c;
        ++b->size;
        if (b->size == b->capacity) {
            char *new_mem = realloc(b->buffer, b->capacity * 2);
            if (!new_mem) {
                return -1;
            }
            memset(new_mem + b->capacity, 0, b->capacity);
            b->buffer = new_mem;
            b->capacity *= 2;
        }
        ++c;
    }
    return 0;
}

char *sb_toString(stringbuilder *b)
{
    return b->buffer;
}

int sb_free(stringbuilder *b)
{
    if (!b)
    {
        return -1;
    }
    free(b->buffer);
    b->capacity = 0;
    b->size = 0;
    return 0;
}