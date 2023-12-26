//
// Created by Bradford, Ben on 26/12/2023.
//

#ifndef HTTPSERVER_HTTPCONNECTION_H
#define HTTPSERVER_HTTPCONNECTION_H

typedef struct sHttpServer HttpServer;

typedef struct sHttpConnection
{
    HttpServer *serv;
    int clientFileDescriptor;
} HttpConnection;

void *httpConnection_handle(void *arg);

#endif //HTTPSERVER_HTTPCONNECTION_H
