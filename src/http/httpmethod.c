//
// Created by Bradford, Ben on 18/12/2023.
//
#include <http/httpmethod.h>
#include <util/common.h>

enum HttpMethod httpMethod_fromString(const char* str)
{
#define RETURN_IF_MATCH(s, e) if (strncmp(s, str, len) == 0) return e

    size_t len = strlen(str);
    RETURN_IF_MATCH("GET", HTTP_GET);
    RETURN_IF_MATCH("POST", HTTP_POST);
    RETURN_IF_MATCH("PUT", HTTP_PUT);
    RETURN_IF_MATCH("DELETE", HTTP_DELETE);
    return -1;
#undef RETURN_IF_MATCH
}

const char* httpMethod_toString(enum HttpMethod httpMethod)
{
    switch (httpMethod)
    {
        case HTTP_GET: return "GET";
        case HTTP_POST: return "POST";
        case HTTP_PUT: return "PUT";
        case HTTP_DELETE: return "DELETE";
        default: return NULL;
    }
}