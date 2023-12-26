//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_HTTPREQUEST_H
#define HTTPSERVER_HTTPREQUEST_H

#include <util/common.h>
#include <util/kvpair.h>

typedef struct sServer Server;

typedef struct sHttpRequest
{
    char *path;
    int httpMethod;
    kvpairs params;
    kvpairs headers;
    char *body;
} HttpRequest;

enum HttpRequestCreateResult {
    HTTP_REQUEST_SUCCESS = 0,
    HTTP_REQUEST_NO_METHOD = -1,
    HTTP_REQUEST_NO_PATH = -2,
    HTTP_REQUEST_INVALID_PARAMS,
    HTTP_REQUEST_NO_PROTOCOL,
    HTTP_REQUEST_INVALID_BODY,
    HTTP_REQUEST_INVALID_HEADER,
};


int httpRequest_init(HttpRequest*);
void httpRequest_free(HttpRequest*);
enum HttpRequestCreateResult httpRequest_create(HttpRequest*, char* inputBuffer);


#endif //HTTPSERVER_HTTPREQUEST_H
