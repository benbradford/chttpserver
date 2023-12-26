//
// Created by Bradford, Ben on 19/12/2023.
//

#include <server/serverfunction.h>

ServerFunction *sf_find(ServerFunctions* functions, int httpMethod, const char *name)
{
    const size_t nameLength = strlen(name);
    for (int i = 0; i < functions->size; ++i)
    {
        ServerFunction* function = vector_get(functions, i);
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