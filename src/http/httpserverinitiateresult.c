//
// Created by Bradford, Ben on 27/12/2023.
//
#include "http/httpserverinitiateresult.h"

const char* sir_toString(int result)
{
    switch(result)
    {
        case SERVER_SUCCESS: return "Success";
        case SERVER_NULL: return "Null HttpServer";
        case SERVER_CANNOT_REGISTER_FUNCTIONS: return "Cannot register functions";
        case SERVER_CANNOT_BIND_PORT: return "Unable to bind to port";
        case SERVER_CANNOT_CREATE_SOCKET: return "Unable to create socket";
        case SERVER_CANNOT_LISTEN: return "Unable to listen";
        case SERVER_CANNOT_ALLOCATE: return "Unable to allocate";
        case SERVER_SELECT_ERROR: return "Unable to select";
        case SERVER_ACCEPT_ERROR: return "Unable to accept";
        default:
            return "Unknown";
    }
}