//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef UNTITLED_HTTPREQUEST_H
#define UNTITLED_HTTPREQUEST_H

typedef struct sVector vector;

typedef struct SHttpRequest
{
    const char *path;
    int httpMethod;
    const char *params;
    vector* headers;
    const char *body;
} httpRequest;

int httpRequest_findParameterValue(const httpRequest* request, char *name, char *value, int maxSize);

#endif //UNTITLED_HTTPREQUEST_H
