//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef HTTPSERVER_HTTPMETHOD_H
#define HTTPSERVER_HTTPMETHOD_H

enum HttpMethod {
    HTTP_GET = 0,
    HTTP_PUT = 1,
    HTTP_POST = 2,
    HTTP_DELETE = 3,

    HTTP_MAX = 4
};

int httpMethod_fromStrong(const char* str);
const char* httpMethod_toString(int httpMethod);

#endif //HTTPSERVER_HTTPMETHOD_H
