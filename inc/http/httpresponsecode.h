//
// Created by Bradford, Ben on 27/12/2023.
//

#ifndef HTTPSERVER_HTTPRESPONSECODE_H
#define HTTPSERVER_HTTPRESPONSECODE_H

enum HttpResponseCode
{
    HTTP_RESPONSE_SUCCESS = 200,
    HTTP_RESPONSE_BAD_REQUEST = 400,
    HTTP_RESPONSE_NOT_FOUND = 404,

    HTTP_RESPONSE_INTERNAL_ERROR = 500,
};

const char *httpResponse_toString(enum HttpResponseCode);

#endif //HTTPSERVER_HTTPRESPONSECODE_H
