//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_HTTPRESPONSE_H
#define HTTPSERVER_HTTPRESPONSE_H

#include "util/common.h"
#include "util/kvpair.h"
#include "http/httpcontenttype.h"
#include "http/httpresponsecode.h"

size_t httpResponse_create(const char* statusLine,
                           const char* body,
                           const kvpairs* responseHeaders,
                           enum HttpContentType contentType,
                           char *response,
                           size_t maxLength);

size_t httpResponse_createErrorRequestWithReason(enum HttpResponseCode, const char *reason, char* response, size_t maxLength);

#endif //HTTPSERVER_HTTPRESPONSE_H
