//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef UNTITLED_REQUESTPARSER_H
#define UNTITLED_REQUESTPARSER_H

typedef struct sVector vector;

static const int BODY_SIZE_TOO_LARGE = -1;
static const int REGEX_INVALID_METHOD = -3;
static const int REGEX_COMPILE_FAILURE = -2;
static const int REGEX_NO_MATCH = -1;
static const int REGEX_FIND_OK = 0;

int extractHeadersAndBody(char* buffer,
                          vector* headers,
                          char* body,
                          int maxBodySize);

int extractMethodPathAndParam(const char* buffer,
                              int* method,
                              char *path,
                              char *params,
                              int maxMethodSize);

#endif //UNTITLED_REQUESTPARSER_H
