//
// Created by Bradford, Ben on 19/12/2023.
//

#ifndef HTTPSERVER_HTTPSERVERFUNCTION_H
#define HTTPSERVER_HTTPSERVERFUNCTION_H

#include "util/common.h"
#include "collection/vector.h"

typedef struct sHttpRequest HttpRequest;
typedef struct sHttpResponse HttpResponse;

typedef struct sHttpServerFunction
{
    const char *boundPath;
    int boundHttpMethod;
    int (*boundFunction)(HttpRequest *, HttpResponse *);
} HttpServerFunction;

typedef vector HttpServerFunctions;

HttpServerFunction *sf_find(HttpServerFunctions*, int httpMethod, const char *path);

#endif //HTTPSERVER_HTTPSERVERFUNCTION_H
