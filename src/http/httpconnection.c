//
// Created by Bradford, Ben on 26/12/2023.
//
#include "util/common.h"
#include "http/httpconnection.h"
#include "http/httpserver.h"
#include "http/httprequest.h"
#include "http/httpresponse.h"
#include <sys/socket.h>
#include <unistd.h>

const char *CONNECTION_SUCCESS = "CONNECTION_HANDLER_SUCCESS";
const char *CONNECTION_FAILED = "CONNECTION_HANDLER_FAILED";
const char *CONNECTION_COULD_NOT_SEND = "CONNECTION_HANDLER_COULD_NOT_SEND";
const char *COULD_NOT_ALLOCATE = "CONNECTION_HANDLER_COULD_NOT_ALLOCATE";

const char *httpConnection_handle(HttpConnection* conn)
{
    const char *handlerResult = CONNECTION_FAILED;
    int createResponseResult;
    HttpResponse response;
    char *buffer = calloc(conn->serv->maxPayloadSize, sizeof(char));
    HttpRequest request;

    if ((createResponseResult = httpResponse_init(&response, conn->serv->maxResponseSize)) < 0)
    {
        goto send;
    }
    if (!buffer)
    {
        createResponseResult = httpResponse_createError(HTTP_RESPONSE_INTERNAL_ERROR, "Cannot allocate", &response);
        goto send;
    }
    if (recv(conn->clientFileDescriptor, buffer, conn->serv->maxPayloadSize, 0) <= 0)
    {
        createResponseResult = httpResponse_createError(HTTP_RESPONSE_BAD_REQUEST, "No bytes received", &response);
        goto send;
    }
    if (httpRequest_init(&request) < 0)
    {
        createResponseResult = httpResponse_createError(HTTP_RESPONSE_INTERNAL_ERROR, "Error creating request", &response);
        goto send;
    }
    if (httpRequest_create(&request, buffer) < 0)
    {
        createResponseResult = httpResponse_createError(HTTP_RESPONSE_BAD_REQUEST, "Error creating request", &response);
        goto send;
    }
    HttpServerFunction *sf = sf_find(&conn->serv->functions, request.httpMethod, request.path);
    if (sf == NULL)
    {
        createResponseResult = httpResponse_createError(HTTP_RESPONSE_NOT_FOUND, "Invalid Path", &response);
        goto send;
    }

    handlerResult = CONNECTION_SUCCESS;
    createResponseResult = sf->func(&request, &response);

    send:;
    httpRequest_free(&request);

    if (createResponseResult < 0)
    {
        printf("Error allocating, could not send response\n");
        handlerResult = COULD_NOT_ALLOCATE;
    }
    else if (send(conn->clientFileDescriptor, response.response, response.responseSize, 0) <= 0)
    {
        handlerResult = CONNECTION_COULD_NOT_SEND;
    }

    close(conn->clientFileDescriptor);
    httpResponse_free(&response);
    free(buffer);
    return (void *)handlerResult;
}
