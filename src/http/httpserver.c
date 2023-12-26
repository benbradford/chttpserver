//
// Created by Bradford, Ben on 18/12/2023.
//
#include "http/httpserver.h"
#include "http/httprequest.h"
#include "http/httpconnection.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

const int DEFAULT_MAX_RESPONSE_SIZE = 104857600;
const int DEFAULT_MAX_PAYLOAD_SIZE = 104857600;

const int SERVER_SUCCESS = 0;
const int NULL_SERVER = -1;
const int CREATE_SOCKET_FAILED_CREATING_SOCKET = -2;
const int CREATE_SOCKET_FAILED_BINDING_PORT = -3;
const int CREATE_SOCKET_FAILED_LISTENING = -4;
const int SERVER_CANNOT_ALLOCATE = -5;
const int SERVER_SELECT_ERROR = -6;
const int SERVER_ACCEPT_ERROR = -7;

const char* server_reason(int result)
{
    switch(result)
    {
        case SERVER_SUCCESS: return "Success";
        case NULL_SERVER: return "Null HttpServer";
        case CREATE_SOCKET_FAILED_BINDING_PORT: return "Unable to bind";
        case CREATE_SOCKET_FAILED_CREATING_SOCKET: return "Unable to create socket";
        case CREATE_SOCKET_FAILED_LISTENING: return "Unable to listen";
        case SERVER_CANNOT_ALLOCATE: return "Unable to allocate";
        case SERVER_SELECT_ERROR: return "Unable to select";
        case SERVER_ACCEPT_ERROR: return "Unable to accept";
        default:
            return "Unknown";
    }
}

int server_init(HttpServer *s)
{
    if (vector_init(&s->functions, 12) < 0)
    {
        return SERVER_CANNOT_ALLOCATE;
    }
    s->serverFileDescriptor = -1;
    s->maxResponseSize = DEFAULT_MAX_RESPONSE_SIZE;
    s->maxPayloadSize = DEFAULT_MAX_PAYLOAD_SIZE;
    s->httpErrorResponder.createErrorRequestWithReason = httpResponse_createErrorRequestWithReason;
    s->httpErrorResponder.createNotFound = httpResponse_createNotFound;
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
}

int server_registerHttpFunction(
    HttpServer *s, int httpMethod,
    const char *name,
    size_t(*func)(HttpRequest *, char *))
{
    if (!s)
    {
        return NULL_SERVER;
    }
    HttpServerFunction *f = calloc(1, sizeof(HttpServerFunction));
    f->func = func;
    f->name = name;
    f->httpMethod = httpMethod;
    vector_pushBack(&s->functions, f);
    return SERVER_SUCCESS;
}

int server_registerCreateNotFoundFunction(HttpServer *s, size_t (*createNotFound)(char* response, size_t maxLength))
{
    if (!s)
    {
        return NULL_SERVER;
    }
    s->httpErrorResponder.createNotFound = createNotFound;
    return SERVER_SUCCESS;
}
int server_registerCreateErrorWithReason(HttpServer *s, size_t (*createErrorRequestWithReason)(enum HttpResponseCode, const char *reason, char* response, size_t maxLength))
{
    if (!s)
    {
        return NULL_SERVER;
    }
    s->httpErrorResponder.createErrorRequestWithReason = createErrorRequestWithReason;
    return SERVER_SUCCESS;
}

int server_createAndBindSocket(HttpServer *s, int port)
{
    if (!s)
    {
        return NULL_SERVER;
    }

    struct sockaddr_in server_addr;

    if ((s->serverFileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return CREATE_SOCKET_FAILED_CREATING_SOCKET;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(s->serverFileDescriptor,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        return CREATE_SOCKET_FAILED_BINDING_PORT;
    }

    if (listen(s->serverFileDescriptor, 10) < 0)
    {
        return CREATE_SOCKET_FAILED_LISTENING;
    }

    return SERVER_SUCCESS;
}

int server_acceptLoop(HttpServer *serv)
{
    if (!serv)
    {
        return NULL_SERVER;
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

        if (select (fdmax + 1, &readfds, NULL, NULL, &timeValue) == -1) {
            response = SERVER_SELECT_ERROR;
            goto end;
        }
        for (int fd = 0; fd < (fdmax + 1); fd++) {
            if (FD_ISSET (fd, &readfds) && fd == serv->serverFileDescriptor) {
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int fd_new;
                if ((fd_new = accept(serv->serverFileDescriptor, (struct sockaddr *) &client_addr, &client_addr_len)) == -1)
                {
                    response = SERVER_ACCEPT_ERROR;
                    goto end;
                }
                FD_SET (fd_new, &fds);
                if (fd_new > fdmax)
                {
                    fdmax = fd_new;
                }
                pthread_t thread_id;
                HttpConnection *connection = calloc(1, sizeof(HttpConnection));
                connection->clientFileDescriptor = fd_new;
                connection->serv = serv;
                pthread_create(&thread_id, NULL, httpConnection_handle, connection);
                pthread_detach(thread_id);
            }
        }
    }
    response = SERVER_SUCCESS;
    end:
    serv->serverState = SERVER_NOT_RUNNING;
    return response;
}
