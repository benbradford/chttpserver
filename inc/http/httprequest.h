//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_HTTPREQUEST_H
#define HTTPSERVER_HTTPREQUEST_H

typedef struct sVector kvpairs;

typedef struct sHttpRequest
{
    const char *path;
    int httpMethod;
    kvpairs *params;
    kvpairs* headers;
    const char *body;
} httpRequest;

#endif //HTTPSERVER_HTTPREQUEST_H
