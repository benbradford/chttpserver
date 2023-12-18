//
// Created by Bradford, Ben on 18/12/2023.
//

#ifndef UNTITLED_HTTPMETHOD_H
#define UNTITLED_HTTPMETHOD_H

enum HttpMethod {
    HTTP_GET = 0,
    HTTP_PUT = 1,
    HTTP_POST = 2,
    HTTP_DELETE = 3,

    HTTP_MAX = 4
};

int httpMethodFromString(const char* str);
const char* httpMethodString(int httpMethod);

#endif //UNTITLED_HTTPMETHOD_H
