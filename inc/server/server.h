//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef UNTITLED_SERVER_H
#define UNTITLED_SERVER_H


#include <util/common.h>
#include <http/httpmethod.h>

typedef struct SHttpRequest httpRequest;

static const int MAX_SERVER_FUNCTIONS = 5;
typedef struct SServerFunction
{
    const char *name;
    int method;
    size_t (*func)(httpRequest *, char *);
} serverFunction;

typedef struct SServer
{
    serverFunction functions[MAX_SERVER_FUNCTIONS];
    int numFunctions;
    int server_fd;

    int maxResponseSize;
    int maxPayloadSize;
    int maxParamsSize;
    int maxPathSize;
    int maxMethodSize;
} server;

int server_init(server *s);
/*
int server_setMaxResponseSize(server *s, int size);
int server_setMaxPayloadSize(server *s, int size);
int server_setMaxParamsSize(server *s, int size);
int server_setMaxPathSize(server *s, int size);
int server_setMaxMethodSize(server *s, int size);
*/
int server_free(server *s);
int server_createAndBindSocket(server *s, int port);
int server_registerHttpFunction(
        server *s,
        int httpMethod,
        const char *name,
        size_t (*func)(httpRequest *, char *));
int server_acceptLoop(server *s);


#endif //UNTITLED_SERVER_H
