//
// Created by Bradford, Ben on 27/12/2023.
//
#include "http/httpresponsecode.h"
#include "util/common.h"

const char* httpResponse_toString(enum HttpResponseCode httpResponse)
{
    switch (httpResponse)
    {
        case 200: return "200 Success";
        case 400: return "400 Bad Request";
        case 404: return "404 Not Found";
        case 500: return "500 Internal Error";
        default: return "Unknown Response Code";
    }
}
