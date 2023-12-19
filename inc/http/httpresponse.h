//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_HTTPRESPONSE_H
#define HTTPSERVER_HTTPRESPONSE_H

#include <util/common.h>
#include <util/kvpair.h>

size_t httpResponse_create(const char* statusLine,
                           const char* body,
                           const kvpairs* responseHeaders,
                           char *response,
                           const size_t maxLength);

size_t httpResponse_createNotFound(char* response, const size_t maxLength);
size_t httpResponse_createInternalError(char* response, const size_t maxLength);
size_t httpResponse_createBadRequest(char* response, const size_t maxLength);

#endif //HTTPSERVER_HTTPRESPONSE_H
