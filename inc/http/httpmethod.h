//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_HTTPMETHOD_H
#define HTTPSERVER_HTTPMETHOD_H

enum HttpMethod {
    HTTP_GET = 0,
    HTTP_PUT = 1,
    HTTP_POST = 2,
    HTTP_DELETE = 3
};

enum HttpMethod httpMethod_fromString(const char* str);
const char* httpMethod_toString(enum HttpMethod);

#endif //HTTPSERVER_HTTPMETHOD_H
