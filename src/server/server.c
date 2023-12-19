//
// Created by Bradford, Ben on 18/12/2023.
//
#include <server/server.h>
#include <http/httpresponse.h>
#include <server/requestparser.h>
#include <http/httprequest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

const int SERVER_SUCCESS = 0;
const int NULL_SERVER = -99;
const int CREATE_SOCKET_FAILED_CREATING_SOCKET = -1;
const int CREATE_SOCKET_FAILED_BINDING_PORT = -2;
const int CREATE_SOCKET_FAILED_LISTENING = -3;

typedef struct SClientConnection
{
    server *serv;
    int client_fd;
} clientConnection;


void *findFunction(server *serv, const int method, const char *path);
void *clientConnectionHandler(void *arg);

int server_init(server *s)
{
    s->server_fd = -1;
    s->numFunctions = 0;
    s->maxResponseSize = 104857600;
    s->maxPayloadSize = 104857600;
    s->maxParamsSize = 2048;
    s->maxPathSize = 5096;
    s->maxMethodSize = 5096;
    return SERVER_SUCCESS;
}

int server_setMaxResponseSize(server *s, int size)
{
  if (!s)
  {
    return -1;
  }
  s->maxResponseSize = size;
  return 0;
}

int server_setMaxPayloadSize(server *s, int size)
{
  if (!s)
  {
    return -1;
  }
  s->maxPayloadSize = size;
  return 0;
}

int server_setMaxParamsSize(server *s, int size)
{
  if (!s)
  {
    return -1;
  }
  s->maxParamsSize = size;
  return 0;
}

int server_setMaxPathSize(server *s, int size)
{
  if (!s)
  {
    return -1;
  }
  s->maxPathSize = size;
  return 0;
}

int server_setMaxMethodSize(server *s, int size)
{
  if (!s)
  {
    return -1;
  }
  s->maxMethodSize = size;
  return 0;
}

int server_free(server *s)
{
    if (!s)
    {
        return NULL_SERVER;
    }

    close(s->server_fd);
    return SERVER_SUCCESS;
}

int server_registerHttpFunction(
        server *s, int httpMethod,
        const char *name,
        size_t(*func)(httpRequest *, char *))
{
    if (!s)
    {
        return NULL_SERVER;
    }
    serverFunction f;
    f.func = func;
    f.name = name;
    f.method = httpMethod;
    s->functions[s->numFunctions++] = f;
    return SERVER_SUCCESS;
}

int server_createAndBindSocket(server *s, int port)
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

int server_acceptLoop(server *serv)
{
    if (!serv)
    {
        return NULL_SERVER;
    }
    serv->isRunning = 1;
    while (serv->isRunning)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(serv->server_fd,
                               (struct sockaddr *)&client_addr,
                               &client_addr_len);

        if (client_fd < 0)
        {
            perror("accept failed");
            continue;
        }

        pthread_t thread_id;
        clientConnection *connection = malloc(sizeof(clientConnection));
        connection->client_fd = client_fd;
        connection->serv = serv;
        pthread_create(&thread_id, NULL, clientConnectionHandler, connection);
        pthread_detach(thread_id);
    }
    close(serv->server_fd);
    return SERVER_SUCCESS;
}


void *findFunction(server *serv, const int method, const char *path)
{
    const int pathLen = strlen(path);
    for (int i = 0; i < serv->numFunctions; ++i)
    {
        const char *comp = serv->functions[i].name;
        const int compLen = strlen(comp);
        if (pathLen != compLen)
        {
            continue;
        }
        if (serv->functions[i].method == method && strncmp(path, comp, pathLen) == 0)
        {
            return serv->functions[i].func;
        }
    }
    return NULL;
}

void *clientConnectionHandler(void *arg)
{
    clientConnection* conn = (clientConnection*)arg;

    char *buffer = (char *)malloc(conn->serv->maxPayloadSize * sizeof(char));

    ssize_t bytes_received = recv(conn->client_fd, buffer, conn->serv->maxPayloadSize, 0);
    char *response = (char *)malloc(conn->serv->maxResponseSize * sizeof(char));
    size_t responseLength = 0;
    if (bytes_received > 0)
    {
        char *path = malloc(sizeof(char) * conn->serv->maxPathSize);
        kvpairs params;
        vector_init(&params, 8);
        int method = 0;
        int findRes = extractMethodPathAndParam(buffer, &method, path, &params, conn->serv->maxMethodSize);
        vector headers;
        vector_init(&headers, 8);
        int maxBodySize = conn->serv->maxResponseSize;
        char* body = malloc(maxBodySize * sizeof(char));
        int findHeaders = extractHeadersAndBody(buffer, &headers, body, maxBodySize);

        if (findRes < 0 || findHeaders < 0)
        {
            responseLength = httpResponse_createBadRequest(response, conn->serv->maxResponseSize);
        }
        else
        {
            size_t (*func)(httpRequest *, char *) = findFunction(conn->serv, method, path);
            if (func)
            {
                httpRequest request;
                request.path = path;
                request.httpMethod = method;
                request.params = &params;
                request.headers = &headers;
                request.body = body;
                responseLength = func(&request, response);
            }
            else
            {
                responseLength = httpResponse_createNotFound(response, conn->serv->maxResponseSize);
            }
        }
        free(body);
        free(path);
        for (int i = 0; i < headers.size; ++i)
        {
            kvpair *header = vector_get(&headers, i);
            free((char *)header->name);
            free((char *)header->value);
            free(header);
        }
        for (int i = 0; i < params.size; ++i)
        {
            kvpair *param = vector_get(&params, i);
            free((char *)param->name);
            free((char *)param->value);
            free(param);
        }
        vector_free(&headers);

    }
    else
    {
        responseLength = httpResponse_createNotFound(response, conn->serv->maxResponseSize);
    }

    send(conn->client_fd, response, responseLength, 0);
    close(conn->client_fd);
    free(response);
    free(buffer);
    free(conn);

    return NULL;
}
