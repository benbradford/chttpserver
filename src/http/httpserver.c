//
// Created by Bradford, Ben on 18/12/2023.
//
#include "http/httpserver.h"
#include "http/httprequest.h"
#include "http/httpresponse.h"
#include "http/httpconnection.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

const int DEFAULT_MAX_RESPONSE_SIZE = 104857600;
const int DEFAULT_MAX_PAYLOAD_SIZE = 4096;
const int DEFAULT_INITIAL_REQUEST_SIZE = 256;

void *connectionHandler(void *arg);

enum HttpServerInitiateResult server_init(HttpServer *s)
{
    if (vector_init(&s->functions, 12) < 0)
    {
        return SERVER_CANNOT_ALLOCATE;
    }
    s->serverFileDescriptor = -1;
    s->maxResponseSize = DEFAULT_MAX_RESPONSE_SIZE;
    s->maxRequestSize = DEFAULT_MAX_PAYLOAD_SIZE;
    s->initialRequestSize = DEFAULT_INITIAL_REQUEST_SIZE;
    s->serverState = SERVER_NOT_RUNNING;
    s->pollingIntervalInSeconds = 1;
    return SERVER_SUCCESS;
}

void server_free(HttpServer *s)
{
    if (!s)
    {
        return;
    }
    for (int i = 0; i < s->functions.size; ++i)
    {
        free(vector_get(&s->functions, i));
    }
    vector_free(&s->functions);
    close(s->serverFileDescriptor);
    s->serverFileDescriptor = 0;
}

enum HttpServerInitiateResult server_registerHttpFunction(HttpServer *s, enum HttpMethod httpMethod, const char *name, int(*func)(HttpRequest *, HttpResponse *))
{
    if (!s)
    {
        return SERVER_NULL;
    }
    HttpServerFunction *f = malloc(sizeof(HttpServerFunction));
    f->boundFunction = func;
    f->boundPath = name;
    f->boundHttpMethod = httpMethod;
    vector_pushBack(&s->functions, f);
    return SERVER_SUCCESS;
}

enum HttpServerInitiateResult server_createBindAndListen(HttpServer * s, int port, int maxPendingConnections)
{
    if (!s)
    {
        return SERVER_NULL;
    }

    if ((s->serverFileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return SERVER_CANNOT_CREATE_SOCKET;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(s->serverFileDescriptor, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        return SERVER_CANNOT_BIND_PORT;
    }

    if (listen(s->serverFileDescriptor, maxPendingConnections) < 0)
    {
        return SERVER_CANNOT_LISTEN;
    }

    return SERVER_SUCCESS;
}

enum HttpServerInitiateResult server_acceptLoop(HttpServer *serv)
{
    if (!serv)
    {
        return SERVER_NULL;
    }
    serv->serverState = SERVER_RUNNING;
    struct timeval timeValue;
    timeValue.tv_sec = serv->pollingIntervalInSeconds;
    int response;
    while (serv->serverState == SERVER_RUNNING)
    {
        fd_set fds, readfds;
        FD_ZERO (&fds);
        FD_SET (serv->serverFileDescriptor, &fds);
        int fdmax = serv->serverFileDescriptor;
        readfds = fds;

        if (select (fdmax + 1, &readfds, NULL, NULL, &timeValue) == -1)
        {
            response = serv->serverState == SERVER_STOP_REQUESTED ? SERVER_SUCCESS : SERVER_SELECT_ERROR;
            goto end;
        }
        for (int fd = 0; fd < (fdmax + 1); fd++)
        {
            if (FD_ISSET (fd, &readfds) && fd == serv->serverFileDescriptor)
            {
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int fd_new;
                if ((fd_new = accept(serv->serverFileDescriptor, (struct sockaddr *) &client_addr, &client_addr_len)) == -1)
                {
                    response = serv->serverState == SERVER_STOP_REQUESTED ? SERVER_SUCCESS : SERVER_ACCEPT_ERROR;
                    goto end;
                }
                FD_SET (fd_new, &fds);
                if (fd_new > fdmax)
                {
                    fdmax = fd_new;
                }
                pthread_t thread_id;
                HttpConnection *connection = malloc(sizeof(HttpConnection));
                connection->clientFileDescriptor = fd_new;
                connection->serv = serv;
                pthread_create(&thread_id, NULL, connectionHandler, connection);
                pthread_detach(thread_id);
            }
        }
    }
    response = SERVER_SUCCESS;
    end:
    serv->serverState = SERVER_NOT_RUNNING;
    return response;
}

void *connectionHandler(void *arg)
{
    httpConnection_handle(arg);
    free(arg);
    pthread_exit(NULL);
}
