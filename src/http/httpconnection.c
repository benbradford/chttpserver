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

void httpConnection_handle(HttpConnection* conn)
{
    int createResponseResult;
    const int initialBufferSize = conn->serv->initialRequestSize;
    char *receiveBuffer = calloc(initialBufferSize, sizeof(char));
    HttpRequest request;
    HttpResponse response;

    if ((createResponseResult = httpResponse_init(&response, conn->serv->maxResponseSize)) < 0)
    {
        printf("ERROR: Unable to init response\n");
        goto send;
    }
    if (!receiveBuffer)
    {
        createResponseResult = httpResponse_createError(HTTP_RESPONSE_INTERNAL_ERROR, "Cannot allocate", &response);
        goto send;
    }
    size_t bytesReceived = recv(conn->clientFileDescriptor, receiveBuffer, conn->serv->maxRequestSize, MSG_PEEK);

    if (bytesReceived <= 0)
    {
        createResponseResult = httpResponse_createError(HTTP_RESPONSE_BAD_REQUEST, "No bytes available in input receiveBuffer", &response);
        goto send;
    }
    if (bytesReceived > conn->serv->maxRequestSize)
    {
        createResponseResult = httpResponse_createError(HTTP_RESPONSE_BAD_REQUEST, "Input size is too large", &response);
        goto send;
    }
    if (bytesReceived > initialBufferSize)
    {
        free(receiveBuffer);
        receiveBuffer = calloc(bytesReceived, sizeof(char));
    }
    bytesReceived = recv(conn->clientFileDescriptor, receiveBuffer, conn->serv->maxRequestSize, 0);
    if (bytesReceived <= 0)
    {
        createResponseResult = httpResponse_createError(HTTP_RESPONSE_BAD_REQUEST, "No bytes received", &response);
        goto send;
    }
    if (httpRequest_init(&request) < 0)
    {
        createResponseResult = httpResponse_createError(HTTP_RESPONSE_INTERNAL_ERROR, "Error creating request", &response);
        goto send;
    }
    if (httpRequest_create(&request, receiveBuffer) < 0)
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

    createResponseResult = sf->boundFunction(&request, &response);

    send:
    httpRequest_free(&request);

    if (createResponseResult < 0)
    {
        printf("ERROR: did not get a response from bound function\n");
    }
    else if (send(conn->clientFileDescriptor, response.response, response.responseSize, 0) <= 0)
    {
        printf("ERROR: Unable to send to client\n");
    }

    close(conn->clientFileDescriptor);
    httpResponse_free(&response);
    free(receiveBuffer);
}
