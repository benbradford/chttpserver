#include <stdio.h>
#include <http/httpmethod.h>
#include <collection/vector.h>
#include <server/server.h>
#include <http/httprequest.h>
#include <util/kvpair.h>
#include <http/httpresponse.h>

size_t echoRequest(httpRequest *req, char *responseString)
{
    const char* methodString = httpMethod_toString(req->httpMethod);
    kvpair *param = kvpair_find(req->params, "p1");

    int numhttpHeaders = 4;
    kvpairs headers;
    vector_init(&headers ,8);
    kvpair methodHeader = {"method", methodString};
    vector_pushBack(&headers, &methodHeader);

    for (int i = 0; i < req->params->size; ++i)
    {
        vector_pushBack(&headers, vector_get(req->params, i));
    }
    for (int i = 0; i < req->headers->size; ++i)
    {
        vector_pushBack(&headers, vector_get(req->headers, i));
    }

    int size = httpResponse_create("HTTP/1.1 200 Success",
                                   "200 Success",
                                   &headers,
                                   responseString, 104857600);

    vector_free(&headers);
    return size;
}


int main() {

    server serv;
    server_init(&serv);
    server_createAndBindSocket(&serv, 8081);

    server_registerHttpFunction(&serv, HTTP_GET, "echoRequest", echoRequest);

    server_acceptLoop(&serv);
    server_free(&serv);
    return 0;
}
