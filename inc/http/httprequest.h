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

int httpRequest_init(HttpRequest*);
int httpRequest_free(HttpRequest*);
int httpRequest_create(HttpRequest*, char* inputBuffer);

#endif //HTTPSERVER_HTTPREQUEST_H
