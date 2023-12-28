//
// Created by Bradford, Ben on 27/12/2023.
//
#include "http/httpcontenttype.h"

const char* httpContentType_toString(enum HttpContentType contentType)
{
    switch (contentType)
    {
        case CONTENT_TYPE_PLAIN_TEXT: return "text/plain";
        case CONTENT_TYPE_JSON: return "text/json";
        default:
            return "mime/unknown";
    }
}