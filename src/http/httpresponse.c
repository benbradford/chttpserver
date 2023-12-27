//
// Created by Bradford, Ben on 18/12/2023.
//
#include "http/httpresponse.h"
#include "util/stringbuilder.h"

size_t httpResponse_create(const char* statusLine,
                           const char* body,
                           const kvpairs *responseHeaders,
                           enum HttpContentType contentType,
                           char *response,
                           size_t maxLength)
{
    stringbuilder s;
    sb_init(&s, statusLine);
    sb_newLine(&s);
    for (int i = 0; i < responseHeaders->size; ++i)
    {
        kvpair *pair = vector_get(responseHeaders, i);
        sb_appendAll(&s, 4, pair->name, ": ", pair->value, "\n");
    }

    sb_appendAll(&s, 3, "Content-Type: ", httpContentType_toString(contentType), "\n");

    if (body)
    {
        char *lenString = calloc(20, sizeof(char));
        snprintf(lenString, 20, "%lu", strlen(body)+1);
        sb_appendAll(&s, 5, "Content-Length: ", lenString, "\n\n", body, "\n");
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

    snprintf(response, len+1, "%s", res);
    size_t size = s.size;
    sb_free(&s);
    return size;
}

size_t httpResponse_createErrorRequestWithReason(enum HttpResponseCode httpResponseCode, const char *reason, char* response, size_t maxLength)
{
    kvpairs headers;
    vector_init(&headers, 2);
    const char *error = httpResponse_toString(httpResponseCode);
    stringbuilder status;
    sb_init(&status, "HTTP/1.1 ");
    sb_append(&status, error);
    stringbuilder body;
    sb_init(&body, error);
    sb_appendAll(&body, 2, " - ", reason);

    size_t size = httpResponse_create(status.buffer,
                                      body.buffer,
                                      &headers,
                                      CONTENT_TYPE_PLAIN_TEXT,
                                      response,
                                      maxLength);
    sb_free(&body);
    sb_free(&status);
    vector_free(&headers);
    return size;
}