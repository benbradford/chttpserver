#include <http/httpmethod.h>
#include <collection/vector.h>
#include <server/server.h>
#include <http/httprequest.h>
#include <util/kvpair.h>
#include <http/httpresponse.h>

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

    server serv;
    server_init(&serv);
    server_createAndBindSocket(&serv, 8082);
    server_registerCreateErrorWithReason(&serv, httpResponse_createErrorRequestWithReason);
    server_registerCreateNotFoundFunction(&serv, httpResponse_createNotFound);
    server_registerHttpFunction(&serv, HTTP_GET, "echoRequest", echoRequest);
    server_acceptLoop(&serv);
    server_free(&serv);

    return 0;
}
