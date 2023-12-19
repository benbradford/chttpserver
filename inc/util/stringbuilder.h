//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_STRINGBUILDER_H
#define HTTPSERVER_STRINGBUILDER_H

#include <util/common.h>

typedef struct sStringBuilder stringbuilder;

struct sStringBuilder {
    char *mem;
    size_t count;
    size_t cap;
};

int sb_init(stringbuilder *, const char *);
int sb_append(stringbuilder *, const char *);
int sb_appendNewLine(stringbuilder *b, const char *c);
int sb_newLine(stringbuilder *b);
char *sb_toString(stringbuilder *);
int sb_free(stringbuilder *);

#endif //HTTPSERVER_STRINGBUILDER_H
