//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef UNTITLED_STRINGBUILDER_H
#define UNTITLED_STRINGBUILDER_H

#include <util/common.h>

typedef struct StringbuilderS stringbuilder;

struct StringbuilderS {
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

#endif //UNTITLED_STRINGBUILDER_H
