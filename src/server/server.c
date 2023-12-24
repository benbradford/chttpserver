//
// Created by Bradford, Ben on 18/12/2023.
//
#include <server/server.h>
#include <http/httprequest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

const int DEFAULT_MAX_RESPONSE_SIZE = 104857600;
const int DEFAULT_MAX_PAYLOAD_SIZE = 104857600;
const int DEFAULT_MAX_PARAMETER_SIZE = 2048;
const int DEFAULT_MAX_PATH_SIZE = 2048;
const int DEFAULT_MAX_HEADER_SIZE = 2048;
const int DEFAULT_MAX_METHOD_SIZE = 64;

const int SERVER_SUCCESS = 0;
const int NULL_SERVER = -99;
const int CREATE_SOCKET_FAILED_CREATING_SOCKET = -1;
const int CREATE_SOCKET_FAILED_BINDING_PORT = -2;
const int CREATE_SOCKET_FAILED_LISTENING = -3;

typedef struct SClientConnection
{
    Server *serv;
    int client_fd;
} clientConnection;

void *clientConnectionHandler(void *arg);

int server_init(Server *s)
{
    int vectorInitRes = vector_init(&s->functions, 12);
    if (vectorInitRes < 0)
    {
        return vectorInitRes;
    }
    s->server_fd = -1;
    s->maxResponseSize = DEFAULT_MAX_RESPONSE_SIZE;
    s->maxPayloadSize = DEFAULT_MAX_PAYLOAD_SIZE;
    s->maxParamsSize = DEFAULT_MAX_PARAMETER_SIZE;
    s->maxPathSize = DEFAULT_MAX_PATH_SIZE;
    s->maxMethodSize = DEFAULT_MAX_METHOD_SIZE;
    s->maxHeaderSize = DEFAULT_MAX_HEADER_SIZE;
    s->httpErrorResponder.createErrorRequestWithReason = httpResponse_createErrorRequestWithReason;
    s->httpErrorResponder.createNotFound = httpResponse_createNotFound;
    return SERVER_SUCCESS;
}

int server_free(Server *s)
{
    if (!s)
    {
        return NULL_SERVER;
    }
    for (int i = 0; i < s->functions.size; ++i)
    {
        free(vector_get(&s->functions, i));
    }
    vector_free(&s->functions);
    close(s->server_fd);
    return SERVER_SUCCESS;
}

int server_registerHttpFunction(
    Server *s, int httpMethod,
    const char *name,
    size_t(*func)(HttpRequest *, char *))
{
    if (!s)
    {
        return NULL_SERVER;
    }
    ServerFunction *f = calloc(1, sizeof(ServerFunction));
    f->func = func;
    f->name = name;
    f->method = httpMethod;
    vector_pushBack(&s->functions, f);
    return SERVER_SUCCESS;
}

int server_registerCreateNotFoundFunction(Server *s, size_t (*createNotFound)(char* response, size_t maxLength))
{
    if (!s)
    {
        return NULL_SERVER;
    }
    s->httpErrorResponder.createNotFound = createNotFound;
    return SERVER_SUCCESS;
}
int server_registerCreateErrorWithReason(Server *s, size_t (*createErrorRequestWithReason)(enum HttpResponseCode, const char *reason, char* response, size_t maxLength))
{
    if (!s)
    {
        return NULL_SERVER;
    }
    s->httpErrorResponder.createErrorRequestWithReason = createErrorRequestWithReason;
    return SERVER_SUCCESS;
}

int server_createAndBindSocket(Server *s, int port)
{
    if (!s)
    {
        return NULL_SERVER;
    }

    struct sockaddr_in server_addr;

    if ((s->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return CREATE_SOCKET_FAILED_CREATING_SOCKET;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(s->server_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        return CREATE_SOCKET_FAILED_BINDING_PORT;
    }

    if (listen(s->server_fd, 10) < 0)
    {
        return CREATE_SOCKET_FAILED_LISTENING;
    }

    return SERVER_SUCCESS;
}

int server_acceptLoop(Server *serv)
{
    if (!serv)
    {
        return NULL_SERVER;
    }
    serv->isRunning = 1;
    while (serv->isRunning > 0)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(serv->server_fd,
                               (struct sockaddr *)&client_addr,
                               &client_addr_len);

        if (client_fd < 0)
        {
            perror("accept failed -");
            continue;
        }

        pthread_t thread_id;
        clientConnection *connection = calloc(1, sizeof(clientConnection));
        connection->client_fd = client_fd;
        connection->serv = serv;
        pthread_create(&thread_id, NULL, clientConnectionHandler, connection);
        pthread_detach(thread_id);
    }
    serv->isRunning = -1;
    return SERVER_SUCCESS;
}

const char *CONNECTION_SUCCESS = "CONNECTION SUCCESS";
const char *CONNECTION_FAILED = "CONNECTION_FAILED";

void *clientConnectionHandler(void *arg)
{
    const char *handlerResult = CONNECTION_FAILED;
    clientConnection* conn = (clientConnection*)arg;

    char *buffer = calloc(conn->serv->maxPayloadSize, sizeof(char));
    char *response = calloc(conn->serv->maxResponseSize, sizeof(char));
    size_t responseLength;
    if (!buffer || !response)
    {
        responseLength = conn->serv->httpErrorResponder.createErrorRequestWithReason(INTERNAL_ERROR, "Cannot allocate", response, conn->serv->maxResponseSize);
        goto send;
    }

    if (recv(conn->client_fd, buffer, conn->serv->maxPayloadSize, 0) <= 0)
    {
        responseLength = conn->serv->httpErrorResponder.createErrorRequestWithReason(BAD_REQUEST, "No bytes received", response, conn->serv->maxResponseSize);
        goto send;
    }
    HttpRequest request;
    if (httpRequest_init(&request) < 0)
    {
        responseLength = conn->serv->httpErrorResponder.createErrorRequestWithReason(INTERNAL_ERROR, "Error creating request", response, conn->serv->maxResponseSize);
        goto send;
    }
    int requestResult = httpRequest_create(&request, buffer);
    if (requestResult < 0)
    {
        responseLength = conn->serv->httpErrorResponder.createErrorRequestWithReason(BAD_REQUEST, "Error creating request", response, conn->serv->maxResponseSize);
        goto send;
    }
    ServerFunction *sf = sf_find(&conn->serv->functions, request.httpMethod, request.path);
    if (sf == NULL)
    {
        responseLength = conn->serv->httpErrorResponder.createErrorRequestWithReason(NOT_FOUND, "Invalid Path", response, conn->serv->maxResponseSize);
        goto send;
    }

    handlerResult = CONNECTION_SUCCESS;

    responseLength = sf->func(&request, response);
    httpRequest_free(&request);

    send:
    send(conn->client_fd, response, responseLength, 0);

    close(conn->client_fd);
    free(response);
    free(buffer);
    free(conn);

    return (void *)handlerResult;
}
