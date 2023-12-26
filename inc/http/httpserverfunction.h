//
// Created by Bradford, Ben on 19/12/2023.
//

#ifndef HTTPSERVER_HTTPSERVERFUNCTION_H
#define HTTPSERVER_HTTPSERVERFUNCTION_H

#include "util/common.h"
#include "collection/vector.h"

typedef struct sHttpRequest HttpRequest;

typedef struct sHttpServerFunction
{
    const char *name;
    int httpMethod;
    size_t (*func)(HttpRequest *, char *);
} HttpServerFunction;

typedef vector HttpServerFunctions;

HttpServerFunction *sf_find(HttpServerFunctions*, int httpMethod, const char *name);

#endif //HTTPSERVER_HTTPSERVERFUNCTION_H
