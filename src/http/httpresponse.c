//
// Created by Bradford, Ben on 18/12/2023.
//
#include "http/httpresponse.h"
#include "util/stringbuilder.h"

int httpResponse_init(HttpResponse * r, size_t maxResponseSize)
{
    if (!r)
    {
        return -1;
    }

    r->maxResponseSize = maxResponseSize;
    r->response = calloc(maxResponseSize, sizeof(char));
    r->responseSize = 0;

    return 0;
}

void httpResponse_free(HttpResponse * r)
{
    if (!r)
    {
        return;
    }
    free(r->response);
}

int httpResponse_create(const char *statusLine,
                        const char *body,
                        const kvpairs *responseHeaders,
                        enum HttpContentType contentType,
                        HttpResponse* r)
{
    if (!r)
    {
        return -1;
    }
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
        char *lenString;
        if ((lenString = calloc(20, sizeof(char))))
        {
            snprintf(lenString, 20, "%lu", strlen(body) + 1);
            sb_appendAll(&s, 5, "Content-Length: ", lenString, "\n\n", body, "\n");
            free(lenString);
        }
    }
    else
    {
        sb_newLine(&s);
    }

    char *res = sb_toString(&s);
    size_t len = strlen(res);
    if (len >= r->maxResponseSize)
    {
        printf("Warning - Truncating Response\n");
        len = r->maxResponseSize - 1;
    }
    res[len] = '\0';

    snprintf(r->response, len+1, "%s", res);
    sb_free(&s);
    r->responseSize = len;
    return 0;
}

int httpResponse_createError(enum HttpResponseCode responseCode, const char *reason, HttpResponse * r)
{
    kvpairs headers;
    vector_init(&headers, 2);
    const char *error = httpResponse_toString(responseCode);
    stringbuilder status;
    sb_init(&status, "HTTP/1.1 ");
    sb_append(&status, error);
    stringbuilder body;
    sb_init(&body, error);
    sb_appendAll(&body, 2, " - ", reason);

    int result = httpResponse_create(status.buffer,
                                      body.buffer,
                                      &headers,
                                      CONTENT_TYPE_PLAIN_TEXT,
                                      r);
    sb_free(&body);
    sb_free(&status);
    vector_free(&headers);
    return result;
}
