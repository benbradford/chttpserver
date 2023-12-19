//
// Created by Bradford, Ben on 18/12/2023.
//
#include <http/httpresponse.h>
#include <util/kvpair.h>
#include <util/stringbuilder.h>

size_t httpResponse_create(const char* statusLine,
                           const char* body,
                           const kvpairs *responseHeaders,
                           char *response,
                           const size_t maxLength)
{
    stringbuilder s;
    sb_init(&s, statusLine);
    sb_newLine(&s);
    for (int i = 0; i < responseHeaders->size; ++i)
    {
        kvpair *pair = vector_get(responseHeaders, i);
        sb_append(&s, pair->name);
        sb_append(&s, ": ");
        sb_appendNewLine(&s, pair->value);
    }

    if (body)
    {
        char *lenString = malloc(sizeof(char) * 20);
        snprintf(lenString, 20, "%lu", strlen(body)+1);
        sb_append(&s, "Content-Length");
        sb_append(&s, ": ");
        sb_appendNewLine(&s, lenString);
        sb_newLine(&s);
        sb_appendNewLine(&s, body);
        free(lenString);
    }
    else
    {
        sb_newLine(&s);
    }

    char *res = sb_toString(&s);
    int len = strlen(res);
    if (len >= maxLength)
    {
        return -1;
    }
    res[len] = '\0';

    snprintf(response, maxLength, "%s", res);
    int size = s.count;
    sb_free(&s);
    return size;
}

// :TODO: improve this monstrosity
static kvpair plainTextContentHeader;
static kvpairs plainTextContentHeaders;
static bool isInited = false;
static kvpairs *createPlainTextContentHeader()
{
    if (isInited)
    {
        return &plainTextContentHeaders;
    }
    plainTextContentHeader.name = "Content-Type";
    plainTextContentHeader.value = "text/plain";
    vector_pushBack(&plainTextContentHeaders, &plainTextContentHeader);
    return &plainTextContentHeaders;
}

size_t httpResponse_createNotFound(char* response, const size_t maxLength)
{
    return httpResponse_create("HTTP/1.1 404 Not Found",
                               "404 Not Found",
                               createPlainTextContentHeader(),
                               response,
                               maxLength);
}

size_t httpResponse_createInternalError(char* response, const size_t maxLength)
{
    return httpResponse_create("HTTP/1.1 500 Internal Error",
                               "500 Internal Error",
                               createPlainTextContentHeader(),
                               response,
                               maxLength);
}

size_t httpResponse_createBadRequest(char* response, const size_t maxLength)
{
    return httpResponse_create("HTTP/1.1 400 Bad Request",
                               "400 Bad request",
                               createPlainTextContentHeader(),
                               response,
                               maxLength);
}