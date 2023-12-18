#include <stdio.h>
#include <http/httpmethod.h>
#include <collection/vector.h>
#include <server/server.h>
#include <http/httprequest.h>
#include <http/httpheader.h>
#include <http/httpresponse.h>


size_t echoRequest(httpRequest *req, char *responseString)
{
    const char* methodString = httpMethodString(req->httpMethod);
    char *value = malloc(sizeof(char) * 255);
    httpRequest_findParameterValue(req, "p1", value, 255);
    int numhttpHeaders = 4;
    httpHeader* headers[numhttpHeaders];

    httpHeader methodHeader;
    methodHeader.name = "method";
    methodHeader.value = methodString;
    headers[0] = &methodHeader;

    httpHeader paramHeader;
    paramHeader.name = "p1";
    paramHeader.value = value;
    headers[1] = &paramHeader;

    httpHeader* headerHeader = NULL;
    for (int i = 0; i < req->headers->size; ++i) {
        headerHeader = vector_get(req->headers, i);
        if (strcmp(headerHeader->name, "header1") == 0)
        {
            headers[2] = headerHeader;
            break;
        }
    }

    httpHeader bodyHeader;
    bodyHeader.name = "body";
    bodyHeader.value = req->body;
    headers[3] = &bodyHeader;

    int size = createResponse("HTTP/1.1 200 Success",
                              "200 Success",
                              headers,
                              numhttpHeaders,
                              responseString,104857600 );

    free(value);

    return size;
}


int main() {
    server* serv = malloc(sizeof(server));
    server_init(serv);
    server_createAndBindSocket(serv, 8081);

    server_registerHttpFunction(serv, HTTP_GET, "echoRequest", echoRequest);

    server_acceptLoop(serv);
    server_free(serv);
    free(serv);
    return 0;
}
