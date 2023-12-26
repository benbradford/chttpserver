//
// Created by Bradford, Ben on 19/12/2023.
//

#include "http/httpserverfunction.h"

HttpServerFunction *sf_find(HttpServerFunctions* functions, int httpMethod, const char *name)
{
    const size_t nameLength = strlen(name);
    for (int i = 0; i < functions->size; ++i)
    {
        HttpServerFunction* function = vector_get(functions, i);
        const size_t compLen = strlen(function->name);
        if (nameLength != compLen)
        {
            continue;
        }
        if (function->httpMethod == httpMethod && strncmp(name, function->name, nameLength) == 0)
        {
            return function;
        }
    }
    return NULL;
}