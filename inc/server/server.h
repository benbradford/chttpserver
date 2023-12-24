//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_SERVER_H
#define HTTPSERVER_SERVER_H

#include <util/common.h>
#include <http/httpmethod.h>
#include <server/serverfunction.h>
#include <http/httpresponse.h>

typedef struct sServer
{
    serverFunctions functions;
    int server_fd;

    size_t maxResponseSize;
    size_t maxPayloadSize;
    size_t maxParamsSize;
    size_t maxHeaderSize;
    size_t maxPathSize;
    size_t maxMethodSize;
    HttpErrorResponder httpErrorResponder;
    volatile int isRunning;
} Server;

int server_init(Server *s);
int server_free(Server *s);
int server_createAndBindSocket(Server *s, int port);
int server_registerHttpFunction(
    Server *s,
    int httpMethod,
    const char *name,
    size_t (*func)(HttpRequest *, char *));
int server_registerCreateNotFoundFunction(Server *, size_t (*)(char* response, size_t maxLength));
int server_registerCreateErrorWithReason(Server *, size_t (*)(enum HttpResponseCode, const char *reason, char* response, size_t maxLength));
int server_acceptLoop(Server *s);

#endif //HTTPSERVER_SERVER_H
