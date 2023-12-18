//
// Created by Bradford, Ben on 18/12/2023.
//
#include <http/httpresponse.h>
#include <http/httpheader.h>
#include <util/stringbuilder.h>


size_t createResponse(const char* statusLine,
                      const char* body,
                      httpHeader *const *httpHeaders,
                      int numhttpHeaders,
                      char *response,
                      const size_t maxLength)
{
    stringbuilder s;
    sb_init(&s, statusLine);
    sb_newLine(&s);
    for (int i = 0; i < numhttpHeaders; ++i)
    {
        sb_append(&s, httpHeaders[i]->name);
        sb_append(&s, ": ");
        sb_appendNewLine(&s, httpHeaders[i]->value);
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

static httpHeader plainTextContentHeader;
static httpHeader *plainTextContentHeaders[] = {&plainTextContentHeader};
static httpHeader *const *createPlainTextContentHeader()
{
    plainTextContentHeader.name = "Content-Type";
    plainTextContentHeader.value = "text/plain";
    return plainTextContentHeaders;
}

size_t createNotFoundRespose(char* response, const size_t maxLength)
{
    return createResponse("HTTP/1.1 404 Not Found",
                          "404 Not Found",
                          createPlainTextContentHeader(),
                          1,
                          response,
                          maxLength);
}

size_t createInternalErrorResponse(char* response, const size_t maxLength)
{
    return createResponse("HTTP/1.1 500 Internal Error",
                          "500 Internal Error",
                          createPlainTextContentHeader(),
                          1,
                          response,
                          maxLength);
}

size_t createBadRequestResponse(char* response, const size_t maxLength)
{
    return createResponse("HTTP/1.1 400 Bad Request",
                          "400 Bad request",
                          createPlainTextContentHeader(),
                          1,
                          response,
                          maxLength);
}