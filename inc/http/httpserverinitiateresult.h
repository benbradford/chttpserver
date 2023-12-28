//
// Created by Bradford, Ben on 27/12/2023.
//

#ifndef HTTPSERVER_HTTPSERVERINITIATERESULT_H
#define HTTPSERVER_HTTPSERVERINITIATERESULT_H

enum HttpServerInitiateResult
{
    SERVER_SUCCESS = 0,
    SERVER_NULL = -1,
    SERVER_CANNOT_REGISTER_FUNCTIONS = -2,
    SERVER_CANNOT_CREATE_SOCKET = -3,
    SERVER_CANNOT_BIND_PORT = -4,
    SERVER_CANNOT_LISTEN = -5,
    SERVER_CANNOT_ALLOCATE = -6,
    SERVER_SELECT_ERROR = -7,
    SERVER_ACCEPT_ERROR = -8,
};

const char* sir_toString(enum HttpServerInitiateResult);

#endif //HTTPSERVER_HTTPSERVERINITIATERESULT_H