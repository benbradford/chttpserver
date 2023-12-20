//
// Created by Bradford, Ben on 18/12/2023.
//
#include <http/httpresponse.h>
#include <util/stringbuilder.h>

const char* httpResponse_toString(enum HttpResponseCode httpResponse)
{
    switch (httpResponse)
    {
        case 400: return "400 Bad Request";
        case 404: return "404 Not Found";
        case 500: return "500 Internal Error";
        default: return NULL;
    }
}

size_t httpResponse_create(const char* statusLine,
                           const char* body,
                           const kvpairs *responseHeaders,
                           char *response,
                           size_t maxLength)
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
        printf("Len String %s", lenString);
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
    size_t len = strlen(res);
    if (len >= maxLength)
    {
        return -1;
    }
    res[len] = '\0';

    snprintf(response, maxLength, "%s", res);
    size_t size = s.size;
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
    isInited = true;
    return &plainTextContentHeaders;
}

size_t httpResponse_createNotFound(char* response, size_t maxLength)
{
    return httpResponse_create("HTTP/1.1 404 Not Found",
                               "404 Not Found",
                               createPlainTextContentHeader(),
                               response,
                               maxLength);
}

size_t httpResponse_createErrorRequestWithReason(enum HttpResponseCode httpResponseCode, const char *reason, char* response, size_t maxLength)
{
    const char *error = httpResponse_toString(httpResponseCode);
    stringbuilder status;
    sb_init(&status, "HTTP/1.1 ");
    sb_init(&status, error);
    stringbuilder body;
    sb_init(&body, error);
    sb_append(&body, " - ");
    sb_append(&body, reason);

    size_t size = httpResponse_create(status.buffer,
                                      body.buffer,
                                      createPlainTextContentHeader(),
                                      response,
                                      maxLength);
    sb_free(&body);
    sb_free(&status);
    return size;
}