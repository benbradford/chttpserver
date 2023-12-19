//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_SERVER_H
#define HTTPSERVER_SERVER_H


#include <util/common.h>
#include <http/httpmethod.h>
#include <server/serverfunction.h>


static const int MAX_SERVER_FUNCTIONS = 5;

typedef struct sServer
{
    serverFunction functions[MAX_SERVER_FUNCTIONS];
    int numFunctions;
    int server_fd;

    int maxResponseSize;
    int maxPayloadSize;
    int maxParamsSize;
    int maxPathSize;
    int maxMethodSize;

    int isRunning;
} server;

int server_init(server *s);
int server_setMaxResponseSize(server *s, int size);
int server_setMaxPayloadSize(server *s, int size);
int server_setMaxParamsSize(server *s, int size);
int server_setMaxPathSize(server *s, int size);
int server_setMaxMethodSize(server *s, int size);
int server_free(server *s);
int server_createAndBindSocket(server *s, int port);
int server_registerHttpFunction(
        server *s,
        int httpMethod,
        const char *name,
        size_t (*func)(httpRequest *, char *));
int server_acceptLoop(server *s);

#endif //HTTPSERVER_SERVER_H
