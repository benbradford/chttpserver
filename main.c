#include "example/cars.h"
#include "example/echo.h"
#include "http/httpmethod.h"
#include "http/httpserver.h"
#include <pthread.h>

const int PORT = 8082;
const int MAX_PENDING_CONNECTIONS = 5;
volatile int response = 0;

void *serverLoop(void *args) {
    HttpServer *serv = (HttpServer*)args;
    response = server_acceptLoop(serv);
    return NULL;
}

void startServerAndWaitForQuit(HttpServer* serv)
{
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, serverLoop, serv);
    pthread_detach(thread_id);

    printf("Server Running. Enter q to exit...\n");
    while (response == SERVER_SUCCESS && 'q' != getchar());
    printf("Server Quitting....\n");
    serv->serverState = serv->serverState == SERVER_RUNNING ? SERVER_STOP_REQUESTED : serv->serverState;
    while (serv->serverState == SERVER_RUNNING);
}

int main() {
    // a simple example of adding http functions to get, update and delete example from a json structure held on file
    // the example are not thread-safe.
    if ((response = cars_init()) < 0)
    {
        printf("Unable to init cars %d\n", response);
        return response;
    }
    HttpServer serv;

    if ((response = server_init(&serv)) < SERVER_SUCCESS) goto end;
    if ((response = server_createBindAndListen(&serv, PORT, MAX_PENDING_CONNECTIONS)) < SERVER_SUCCESS) goto end;

    if (server_registerHttpFunction(&serv, HTTP_GET, "echoRequest", echoRequest) < 0 ||
        server_registerHttpFunction(&serv, HTTP_PUT, "cars", cars_add) < 0 ||
        server_registerHttpFunction(&serv, HTTP_GET, "cars", cars_get) < 0 ||
        server_registerHttpFunction(&serv, HTTP_DELETE, "cars", cars_delete) < 0)
    {
        response = SERVER_CANNOT_REGISTER_FUNCTIONS;
        goto end;
    }

    response = SERVER_SUCCESS;

    startServerAndWaitForQuit(&serv);

    end:
    cars_free();
    server_free(&serv);

    printf("Response Code: '%s'\n", sir_toString(response));
    return 0;
}
