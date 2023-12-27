//
// Created by Bradford, Ben on 21/12/2023.
//

#ifndef HTTPSERVER_CARS_H
#define HTTPSERVER_CARS_H

#include "util/common.h"
#include <http/httprequest.h>

int cars_init();
void cars_free();
size_t cars_add(HttpRequest *req, char *responseString);
size_t cars_get(HttpRequest *req, char *responseString);
size_t cars_delete(HttpRequest *req, char *responseString);

#endif //HTTPSERVER_CARS_H
