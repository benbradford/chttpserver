//
// Created by Bradford, Ben on 21/12/2023.
//

#ifndef HTTPSERVER_CARS_H
#define HTTPSERVER_CARS_H

#include "util/common.h"
#include "http/httprequest.h"
#include "http/httpresponse.h"

int cars_init();
void cars_free();
int cars_add(HttpRequest *, HttpResponse *);
int cars_get(HttpRequest *, HttpResponse *);
int cars_delete(HttpRequest *, HttpResponse *);

#endif //HTTPSERVER_CARS_H
