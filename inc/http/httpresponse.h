//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_HTTPRESPONSE_H
#define HTTPSERVER_HTTPRESPONSE_H

#include "util/common.h"
#include "util/kvpair.h"
#include "http/httpcontenttype.h"
#include "http/httpresponsecode.h"

typedef struct sHttpResponse
{
    size_t maxResponseSize;
    char *response;
    size_t responseSize;
} HttpResponse;

int httpResponse_init(HttpResponse *, size_t maxResponseSize);
void httpResponse_free(HttpResponse *);
int httpResponse_create(const char *statusLine,
                        const char *body,
                        const kvpairs *responseHeaders,
                        enum HttpContentType,
                        HttpResponse*);

int httpResponse_createError(enum HttpResponseCode, const char *reason, HttpResponse *);


#endif //HTTPSERVER_HTTPRESPONSE_H
