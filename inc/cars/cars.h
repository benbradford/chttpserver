//
// Created by Bradford, Ben on 21/12/2023.
//

#ifndef HTTPSERVER_CARS_H
#define HTTPSERVER_CARS_H

#include <util/common.h>
#include <http/httprequest.h>

int cars_init();
int cars_free();
size_t cars_add(httpRequest *req, char *responseString);
size_t cars_get(httpRequest *req, char *responseString);

#endif //HTTPSERVER_CARS_H
