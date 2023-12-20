//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_SERVER_H
#define HTTPSERVER_SERVER_H


#include <util/common.h>
#include <http/httpmethod.h>
#include <server/serverfunction.h>

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

    int isRunning;
} server;

int server_init(server *s);
int server_free(server *s);
int server_createAndBindSocket(server *s, int port);
int server_registerHttpFunction(
        server *s,
        int httpMethod,
        const char *name,
        size_t (*func)(httpRequest *, char *));
int server_acceptLoop(server *s);

#endif //HTTPSERVER_SERVER_H
