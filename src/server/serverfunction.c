//
// Created by Bradford, Ben on 19/12/2023.
//

#include <server/serverfunction.h>

serverFunction *sf_find(serverFunctions* functions, int httpMethod, const char *name)
{
    const size_t nameLength = strlen(name);
    for (int i = 0; i < functions->size; ++i)
    {
        serverFunction* function = vector_get(functions, i);
        const size_t compLen = strlen(function->name);
        if (nameLength != compLen)
        {
            continue;
        }
        if (function->method == httpMethod && strncmp(name, function->name, nameLength) == 0)
        {
            return function;
        }
    }
    return NULL;
}