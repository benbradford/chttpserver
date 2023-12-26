#include <http/httpmethod.h>
#include <collection/vector.h>
#include "http/httpserver.h"
#include <http/httprequest.h>
#include <cars/cars.h>
#include <pthread.h>

static const int UNABLE_TO_REGISTER = -10;

size_t echoRequest(HttpRequest *req, char *responseString)
{
    const char* methodString = httpMethod_toString(req->httpMethod);

    kvpairs headers;
    vector_init(&headers ,8);
    kvpair methodHeader = {"httpMethod", methodString};
    kvpair pathHeader = {"path", req->path};
    kvpair bodyHeader = {"body", req->body};
    vector_pushBack(&headers, &methodHeader);
    vector_pushBack(&headers, &pathHeader);
    if (req->body) {
        vector_pushBack(&headers, &bodyHeader);
    }
    vector_addAll(&headers, &req->params);
    vector_addAll(&headers, &req->headers);

    size_t size = httpResponse_create("HTTP/1.1 200 Success",
                                      httpResponse_toString(HTTP_RESPONSE_SUCCESS),
                                   &headers,
                                   responseString, 104857600);

    vector_free(&headers);
    return size;
}

void *serverLoop(void *args) {
    HttpServer *serv = (HttpServer*)args;

    server_acceptLoop(serv);

    return NULL;
}

int main() {

    cars_init();
    HttpServer serv;

    int response = server_init(&serv);
    if (response < 0) goto end;

    response = server_createAndBindSocket(&serv, 8082);
    if (response < 0) goto end;

    if (
        server_registerCreateErrorWithReason(&serv, httpResponse_createErrorRequestWithReason) < 0 ||
        server_registerCreateNotFoundFunction(&serv, httpResponse_createNotFound) < 0 ||

        server_registerHttpFunction(&serv, HTTP_GET, "echoRequest", echoRequest) < 0 ||
        server_registerHttpFunction(&serv, HTTP_PUT, "cars", cars_add) < 0 ||
        server_registerHttpFunction(&serv, HTTP_GET, "cars", cars_get) < 0
        )
    {
        response = UNABLE_TO_REGISTER;
        goto end;
    }

    pthread_t thread_id;

    pthread_create(&thread_id, NULL, serverLoop, &serv);
    pthread_detach(thread_id);

    char c;
    scanf("%c", &c);
    serv.serverState = SERVER_REQUEST_STOPPED;
    while (serv.serverState != SERVER_NOT_RUNNING);
    response = 0;

    end:
    cars_free();
    server_free(&serv);

    printf("HttpServer Response is %s\n", server_reason(response));
    return response;
}
