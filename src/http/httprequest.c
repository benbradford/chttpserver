//
// Created by Bradford, Ben on 24/12/2023.
//
#include "http/httprequest.h"
#include "http/httpmethod.h"
#include <regex.h>

int httpRequest_init(HttpRequest* r)
{
    r->path = NULL;
    int res = vector_init(&r->params, 8);
    res &= vector_init(&r->headers, 8);
    r->body = NULL;
    return res;
}

void httpRequest_free(HttpRequest* r)
{
    free(r->path);
    free(r->body);
    kvpair_freeAll(&r->params);
    kvpair_freeAll(&r->headers);
}

enum HttpRequestCreateResult httpRequest_create(HttpRequest* r, char* inputBuffer)
{
    char *httpMethod = strsep(&inputBuffer, " ");
    r->httpMethod = httpMethod_fromString(httpMethod);
    if (r->httpMethod == -1)
    {
        return HTTP_REQUEST_NO_METHOD;
    }
    char *pathAndParams = strsep(&inputBuffer, " ");

    if (*pathAndParams != '/')
    {
        return HTTP_REQUEST_NO_PATH;
    }
    ++pathAndParams;
    if (!pathAndParams || *pathAndParams == '\0')
    {
        return HTTP_REQUEST_NO_PATH;
    }
    char *path = strsep(&pathAndParams, "?");
    if (!path || *path == '\0')
    {
        return HTTP_REQUEST_NO_PATH;
    }
    r->path = calloc(strlen(path), sizeof(char));
    strcpy(r->path, path);
    if(pathAndParams != NULL)
    {
        char *params = pathAndParams;
        while (params)
        {
            char *c = strsep(&params, "=");
            if (!c)
            {
                return HTTP_REQUEST_INVALID_PARAMS;
            }
            char *name = calloc(strlen(c), sizeof(char));
            strcpy(name, c);
            c = strsep(&params, "& ");
            if (!c)
            {
                free(name);
                return HTTP_REQUEST_INVALID_PARAMS;
            }
            char *val = calloc(strlen(c), sizeof(char));
            strcpy(val, c);
            kvpair *pair = calloc(1, sizeof(kvpair));
            pair->name = name;
            pair->value = val;
            vector_pushBack(&r->params, pair);
        }
    }
    if (!strsep(&inputBuffer, "\n"))
    {
        return HTTP_REQUEST_NO_PROTOCOL;
    }
    if (!inputBuffer)
    {
        return HTTP_REQUEST_INVALID_BODY;
    }
    while (*inputBuffer != '\n' && *inputBuffer != '\r')
    {
        char *headerLine = strsep(&inputBuffer, "\n");
        if (!headerLine)
        {
            return HTTP_REQUEST_INVALID_HEADER;
        }
        regex_t regex;
        if (regcomp(&regex, "^(.*):(.*)", REG_EXTENDED) != 0)
        {
            regfree(&regex);
            return HTTP_REQUEST_INVALID_HEADER;
        }

        regmatch_t matches[3];
        if (regexec(&regex, headerLine, 3, matches, 0) != 0)
        {
            regfree(&regex);
            return HTTP_REQUEST_INVALID_HEADER;
        }
        if (matches[0].rm_so == -1 || matches[1].rm_so == -1 || matches[2].rm_eo == -1)
        {
            return HTTP_REQUEST_INVALID_HEADER;
        }
        size_t startOfHeaderValue = matches[2].rm_eo;
        if (headerLine[startOfHeaderValue] == ' ')
        {
            ++startOfHeaderValue;
        }
        char *name = calloc(matches[1].rm_eo - matches[1].rm_so, sizeof(char));
        char *value = calloc(startOfHeaderValue - matches[2].rm_so-1, sizeof(char));
        strncpy(name, &headerLine[matches[1].rm_so], matches[1].rm_eo - matches[1].rm_so);
        strncpy(value, &headerLine[matches[2].rm_so], startOfHeaderValue - matches[2].rm_so-1);
        kvpair* header = calloc(1, sizeof(kvpair));
        header->name = name;
        header->value = value;
        vector_pushBack(&r->headers, header);
        regfree(&regex);
    }

    char *tok = strsep(&inputBuffer, "\n");
    if (tok == NULL)
    {
        return HTTP_REQUEST_SUCCESS;
    }
    size_t bodySize = strlen(inputBuffer);
    if (bodySize == 0)
    {
        return HTTP_REQUEST_SUCCESS;
    }
    r->body = calloc(bodySize, sizeof(char));
    strcpy(r->body, inputBuffer);

    return HTTP_REQUEST_SUCCESS;
}
