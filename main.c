#include <http/httpmethod.h>
#include <collection/vector.h>
#include <server/server.h>
#include <http/httprequest.h>
#include <cars/cars.h>

static const int UNABLE_TO_REGISTER = -10;

size_t echoRequest(httpRequest *req, char *responseString)
{
    const char* methodString = httpMethod_toString(req->httpMethod);

    kvpairs headers;
    vector_init(&headers ,8);
    kvpair methodHeader = {"method", methodString};
    kvpair pathHeader = {"path", req->path};
    kvpair bodyHeader = {"body", req->body};
    vector_pushBack(&headers, &methodHeader);
    vector_pushBack(&headers, &pathHeader);
    vector_pushBack(&headers, &bodyHeader);
    vector_addAll(&headers, req->params);
    vector_addAll(&headers, req->headers);

    size_t size = httpResponse_create("HTTP/1.1 200 Success",
                                   "200 Success",
                                   &headers,
                                   responseString, 104857600);

    vector_free(&headers);
    return size;
}


int main() {
    cars_init();
    server serv;

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

    response = !server_acceptLoop(&serv);

    end:
    cars_free();
    server_free(&serv);

    printf("Response is %d", response);
    return response;
}
