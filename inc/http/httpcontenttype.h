//
// Created by Bradford, Ben on 27/12/2023.
//

#ifndef HTTPSERVER_HTTPCONTENTTYPE_H
#define HTTPSERVER_HTTPCONTENTTYPE_H

enum HttpContentType {
    CONTENT_TYPE_PLAIN_TEXT = 0,
    CONTENT_TYPE_JSON = 1
};

const char* httpContentType_toString(enum HttpContentType contentType);

#endif //HTTPSERVER_HTTPCONTENTTYPE_H
