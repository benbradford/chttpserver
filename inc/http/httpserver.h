//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_HTTPSERVER_H
#define HTTPSERVER_HTTPSERVER_H

#include "util/common.h"
#include "httpmethod.h"
#include "httpserverfunction.h"
#include "httpserverinitiateresult.h"
#include "httpserverstate.h"

typedef struct sHttpServer
{
    HttpServerFunctions functions;
    int serverFileDescriptor;
    int pollingIntervalInSeconds;
    size_t maxResponseSize;
    size_t maxPayloadSize;
    volatile enum HttpServerState serverState;
} HttpServer;

enum HttpServerInitiateResult server_init(HttpServer *);
enum HttpServerInitiateResult  server_createAndBindSocket(HttpServer *, int port);
enum HttpServerInitiateResult  server_registerHttpFunction(HttpServer *, enum HttpMethod, const char *name, size_t (*func)(HttpRequest *, char *));
enum HttpServerInitiateResult  server_acceptLoop(HttpServer *s);
void server_free(HttpServer *);

#endif //HTTPSERVER_HTTPSERVER_H
