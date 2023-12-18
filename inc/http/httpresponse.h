//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef UNTITLED_HTTPRESPONSE_H
#define UNTITLED_HTTPRESPONSE_H

#include <util/common.h>

typedef struct ShttpHeader httpHeader;

size_t createResponse(const char* statusLine,
                      const char* body,
                      httpHeader *const *httpHeaders,
                      int numhttpHeaders,
                      char *response,
                      const size_t maxLength);

size_t createNotFoundRespose(char* response, const size_t maxLength);
size_t createInternalErrorResponse(char* response, const size_t maxLength);
size_t createBadRequestResponse(char* response, const size_t maxLength);

#endif //UNTITLED_HTTPRESPONSE_H
