//
// Created by Bradford, Ben on 19/12/2023.
//

#ifndef HTTPSERVER_SERVERFUNCTION_H
#define HTTPSERVER_SERVERFUNCTION_H

#include <util/common.h>
#include <collection/vector.h>

typedef struct sHttpRequest httpRequest;

typedef struct sServerFunction
{
    const char *name;
    int method;
    size_t (*func)(httpRequest *, char *);
} serverFunction;

typedef vector serverFunctions;

serverFunction *sf_find(serverFunctions* functions, int httpMethod, const char *name);

#endif //HTTPSERVER_SERVERFUNCTION_H
