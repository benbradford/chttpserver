//
// Created by Bradford, Ben on 26/12/2023.
//
#include <util/common.h>
#include <http/httpconnection.h>
#include <http/httpserver.h>
#include <http/httprequest.h>
#include <sys/socket.h>
#include <unistd.h>

const char *CONNECTION_SUCCESS = "CONNECTION_HANDLER_SUCCESS";
const char *CONNECTION_FAILED = "CONNECTION_HANDLER_FAILED";
const char *CONNECTION_COULD_NOT_SEND = "CONNECTION_HANDLER_COULD_NOT_SEND";

void *httpConnection_handle(void *arg)
{
    const char *handlerResult = CONNECTION_FAILED;
    HttpConnection* conn = (HttpConnection*)arg;

    char *buffer = calloc(conn->serv->maxPayloadSize, sizeof(char));
    char *response = calloc(conn->serv->maxResponseSize, sizeof(char));
    size_t responseLength;
    if (!buffer || !response)
    {
        responseLength = conn->serv->httpErrorResponder.createErrorRequestWithReason(HTTP_RESPONSE_INTERNAL_ERROR, "Cannot allocate", response, conn->serv->maxResponseSize);
        goto send;
    }

    if (recv(conn->clientFileDescriptor, buffer, conn->serv->maxPayloadSize, 0) <= 0)
    {
        responseLength = conn->serv->httpErrorResponder.createErrorRequestWithReason(HTTP_RESPONSE_BAD_REQUEST, "No bytes received", response, conn->serv->maxResponseSize);
        goto send;
    }
    HttpRequest request;
    if (httpRequest_init(&request) < 0)
    {
        responseLength = conn->serv->httpErrorResponder.createErrorRequestWithReason(HTTP_RESPONSE_INTERNAL_ERROR, "Error creating request", response, conn->serv->maxResponseSize);
        goto send;
    }
    int requestResult = httpRequest_create(&request, buffer);
    if (requestResult < HTTP_REQUEST_SUCCESS)
    {
        responseLength = conn->serv->httpErrorResponder.createErrorRequestWithReason(HTTP_RESPONSE_BAD_REQUEST, "Error creating request", response, conn->serv->maxResponseSize);
        goto send;
    }
    HttpServerFunction *sf = sf_find(&conn->serv->functions, request.httpMethod, request.path);
    if (sf == NULL)
    {
        responseLength = conn->serv->httpErrorResponder.createErrorRequestWithReason(HTTP_RESPONSE_NOT_FOUND, "Invalid Path", response, conn->serv->maxResponseSize);
        goto send;
    }

    handlerResult = CONNECTION_SUCCESS;

    responseLength = sf->func(&request, response);
    httpRequest_free(&request);

    send:;
    if (send(conn->clientFileDescriptor, response, responseLength, 0) <= 0)
    {
        handlerResult = CONNECTION_COULD_NOT_SEND;
    }

    close(conn->clientFileDescriptor);
    free(response);
    free(buffer);
    free(conn);
    return (void *)handlerResult;
}
