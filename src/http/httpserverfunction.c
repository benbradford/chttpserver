//
// Created by Bradford, Ben on 19/12/2023.
//

#include "http/httpserverfunction.h"

HttpServerFunction *sf_find(HttpServerFunctions* functions, int httpMethod, const char *path)
{
    const size_t nameLength = strlen(path);
    for (int i = 0; i < functions->size; ++i)
    {
        HttpServerFunction* function = vector_get(functions, i);
        if (function->boundHttpMethod != httpMethod)
        {
            continue;
        }
        const size_t compLen = strlen(function->boundPath);
        if (nameLength != compLen)
        {
            continue;
        }
        if (strncmp(path, function->boundPath, nameLength) == 0)
        {
            return function;
        }
    }
    return NULL;
}