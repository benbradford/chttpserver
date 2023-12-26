//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_HTTPSERVER_H
#define HTTPSERVER_HTTPSERVER_H

#include "util/common.h"
#include "httpmethod.h"
#include "httpserverfunction.h"
#include "httpresponse.h"

enum HttpServerState {
    SERVER_NOT_RUNNING = -1,
    SERVER_RUNNING = 1,
    SERVER_REQUEST_STOPPED = 0
};

typedef struct sHttpServer
{
    HttpServerFunctions functions;
    int serverFileDescriptor;
    int pollingIntervalInSeconds;
    size_t maxResponseSize;
    size_t maxPayloadSize;
    HttpErrorResponder httpErrorResponder;
    volatile enum HttpServerState serverState;
} HttpServer;

int server_init(HttpServer *);
void server_free(HttpServer *);
int server_createAndBindSocket(HttpServer *, int port);
int server_registerHttpFunction(
    HttpServer *s,
    int httpMethod,
    const char *name,
    size_t (*func)(HttpRequest *, char *));
int server_registerCreateNotFoundFunction(HttpServer *, size_t (*)(char* response, size_t maxLength));
int server_registerCreateErrorWithReason(HttpServer *, size_t (*)(enum HttpResponseCode, const char *reason, char* response, size_t maxLength));
int server_acceptLoop(HttpServer *s);
const char* server_reason(int result);

#endif //HTTPSERVER_HTTPSERVER_H
