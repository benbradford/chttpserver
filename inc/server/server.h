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
    ServerFunctions functions;
    int server_fd;

    size_t maxResponseSize;
    size_t maxPayloadSize;
    HttpErrorResponder httpErrorResponder;
    volatile int isRunning;
} Server;

int server_init(Server *s);
void server_free(Server *s);
int server_createAndBindSocket(Server *s, int port);
int server_registerHttpFunction(
    Server *s,
    int httpMethod,
    const char *name,
    size_t (*func)(HttpRequest *, char *));
int server_registerCreateNotFoundFunction(Server *, size_t (*)(char* response, size_t maxLength));
int server_registerCreateErrorWithReason(Server *, size_t (*)(enum HttpResponseCode, const char *reason, char* response, size_t maxLength));
int server_acceptLoop(Server *s);
const char* server_reason(int result);

#endif //HTTPSERVER_SERVER_H
