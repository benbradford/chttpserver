//
// Created by Bradford, Ben on 27/12/2023.
//

#ifndef HTTPSERVER_ECHO_H
#define HTTPSERVER_ECHO_H

#include "util/common.h"
#include "http/httprequest.h"
#include "http/httpresponse.h"

int echoRequest(HttpRequest *req, HttpResponse *response);
#endif //HTTPSERVER_ECHO_H
