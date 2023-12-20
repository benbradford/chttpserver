//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_REQUESTPARSER_H
#define HTTPSERVER_REQUESTPARSER_H

#include <util/common.h>

typedef struct sVector kvpairs;

static const int BODY_SIZE_TOO_LARGE = -1;
static const int REGEX_INVALID_METHOD = -3;
static const int REGEX_COMPILE_FAILURE = -2;
static const int REGEX_NO_MATCH = -1;
static const int REGEX_FIND_OK = 0;

int extractHeadersAndBody(const char* buffer,
                          kvpairs* headers,
                          char* body,
                          size_t maxBodySize,
                          size_t maxHeaderSize);

int extractMethodPathAndParam(const char* buffer,
                              int* method,
                              char *path,
                              kvpairs *params,
                              size_t maxMethodSize,
                              size_t maxParamSize);

#endif //HTTPSERVER_REQUESTPARSER_H
