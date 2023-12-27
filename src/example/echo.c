//
// Created by Bradford, Ben on 27/12/2023.
//
#include "example/echo.h"
#include "http/httpmethod.h"
#include "http/httpresponse.h"

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
                                      CONTENT_TYPE_JSON,
                                      responseString,
                                      104857600); // :TODO

    vector_free(&headers);
    return size;
}
