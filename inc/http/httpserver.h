//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_HTTPSERVER_H
#define HTTPSERVER_HTTPSERVER_H

#include "util/common.h"
#include "httpmethod.h"
#include "httpserverfunction.h"
#include "httpserverinitiateresult.h"

typedef struct sHttpServer
{
    size_t maxResponseSize;     // set to max response size you're willing to accept
    size_t initialRequestSize;  // set to expected max input size
    size_t maxRequestSize;      // set to max input size you're willing to accept

    HttpServerFunctions functions;
    int serverFileDescriptor;
    int pollingIntervalInSeconds;
    volatile enum HttpServerState {
        SERVER_NOT_RUNNING = -1,
        SERVER_STOP_REQUESTED = 0,
        SERVER_RUNNING = 1,
    } serverState;
} HttpServer;

enum HttpServerInitiateResult server_init(HttpServer *);
enum HttpServerInitiateResult server_createBindAndListen(HttpServer *, int port, int maxPendingConnections);
enum HttpServerInitiateResult server_registerHttpFunction(HttpServer *, enum HttpMethod, const char *name, int (*func)(HttpRequest *, HttpResponse *));
enum HttpServerInitiateResult server_acceptLoop(HttpServer *s);
void server_free(HttpServer *);

#endif //HTTPSERVER_HTTPSERVER_H
