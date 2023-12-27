//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_STRINGBUILDER_H
#define HTTPSERVER_STRINGBUILDER_H

#include "util/common.h"

typedef struct sStringBuilder stringbuilder;

struct sStringBuilder {
    char *buffer;
    size_t size;
    size_t capacity;
};

int sb_init(stringbuilder *, const char *);
int sb_append(stringbuilder *, const char *);
int sb_appendAll(stringbuilder *b, int numArgs, ...);
int sb_appendNewLine(stringbuilder *, const char *);
int sb_newLine(stringbuilder *);
char *sb_toString(stringbuilder *);
void sb_free(stringbuilder *);

#endif //HTTPSERVER_STRINGBUILDER_H
