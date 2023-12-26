//
// Created by Bradford, Ben on 19/12/2023.
//

#ifndef HTTPSERVER_SERVERFUNCTION_H
#define HTTPSERVER_SERVERFUNCTION_H

#include <util/common.h>
#include <collection/vector.h>

typedef struct sHttpRequest HttpRequest;

typedef struct sServerFunction
{
    const char *name;
    int httpMethod;
    size_t (*func)(HttpRequest *, char *);
} ServerFunction;

typedef vector ServerFunctions;

ServerFunction *sf_find(ServerFunctions*, int httpMethod, const char *name);

#endif //HTTPSERVER_SERVERFUNCTION_H
