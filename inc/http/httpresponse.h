//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_HTTPRESPONSE_H
#define HTTPSERVER_HTTPRESPONSE_H

#include <util/common.h>
#include <util/kvpair.h>

enum HttpResponseCode
{
    BAD_REQUEST = 400,
    NOT_FOUND = 404,

    INTERNAL_ERROR = 500,
};

const char *httpResponse_toString(enum HttpResponseCode);

size_t httpResponse_create(const char* statusLine,
                           const char* body,
                           const kvpairs* responseHeaders,
                           char *response,
                           size_t maxLength);

size_t httpResponse_createNotFound(char* response, size_t maxLength);
size_t httpResponse_createErrorRequestWithReason(enum HttpResponseCode, const char *reason, char* response, size_t maxLength);

#endif //HTTPSERVER_HTTPRESPONSE_H
