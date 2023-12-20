//
// Created by Bradford, Ben on 18/12/2023.
//
#include <server/requestparser.h>
#include <http/httpmethod.h>
#include <collection/vector.h>

#include <regex.h>
#include "util/kvpair.h"

void url_decode(char *decoded) {
    char *src = decoded;
    size_t src_len = strlen(decoded);
    size_t decoded_len = 0;

    for (size_t i = 0; i < src_len; i++)
    {
        if (src[i] == '%' && i + 2 < src_len)
        {
            int hex_val;
            sscanf(src + i + 1, "%2x", &hex_val);
            decoded[decoded_len++] = (char)hex_val;
            i += 2;
        } else
        {
            decoded[decoded_len++] = src[i];
        }
    }

    decoded[decoded_len] = '\0';

}

kvpair *createParameter(const char* param)
{
    size_t paramLength = strlen(param);
    regex_t regex;
    char *regstring = "^(.*)=(.*)$";
    int res = regcomp(&regex, regstring, REG_EXTENDED);
    if (res == 0)
    {
        regmatch_t matches[3];
        res = regexec(&regex, param, 3, matches, 0);
        if (res == 0 && matches[1].rm_so != -1)
        {
            size_t endOfName = matches[1].rm_eo - matches[1].rm_so;
            if (endOfName > paramLength)
            {
                regfree(&regex);
                return NULL;
            }
            char *value = malloc(sizeof(char) * paramLength);
            char *name = malloc(sizeof(char) * paramLength);
            strncpy(name, param + matches[1].rm_so, endOfName);
            strncpy(value, param + matches[2].rm_so,matches[2].rm_eo - matches[2].rm_so );
            char *tok = strchr(value, '&');
            if (tok != NULL)
            {
                value[(int)(tok - value)] = '\0';
            }
            kvpair *p = malloc(sizeof(kvpair));
            p->name = name;
            p->value = value;
            regfree(&regex);
            return p;
        }
    }
    return NULL;
}

void extractParams(char *path, long locationOfQ, long paramsLen, int maxParamSize, kvpairs *params)
{
    char *paramString = malloc(sizeof(char) * maxParamSize);
    memcpy(paramString, path + locationOfQ + 1, paramsLen );
    path[locationOfQ] = '\0';
    paramString[paramsLen] = '\0';

    char *thisParam = paramString;
    char *nextParam = thisParam;
    while (nextParam)
    {
        nextParam = strchr(thisParam, '&');
        if (nextParam)
        {
            ++nextParam;
            if (thisParam) {
                thisParam[(int) (nextParam - thisParam) - 1] = '\0';
            }
        }

        kvpair *p = createParameter(thisParam);
        if (p)
        {
            vector_pushBack(params, p);
        }
        thisParam = nextParam;

    }

    free(paramString);
}

void extractMethod(const char* buffer, size_t endOfMethod, int *method, size_t maxMethodSize)
{
    char *methodString = malloc(maxMethodSize * sizeof(char));
    strncpy(methodString, buffer, endOfMethod);
    methodString[endOfMethod] = '\0';
    *method = httpMethod_fromString(methodString);
    free(methodString);
}

void extractPath(const char *buffer, size_t matchEnd, size_t matchStart, char *path)
{
    size_t endOfPath = matchEnd - matchStart;
    strncpy(path, buffer + matchStart, endOfPath);
    path[endOfPath] = '\0';
    url_decode(path);
}

int extractMethodPathAndParam(const char* buffer, int* method, char *path, kvpairs *params, size_t maxMethodSize, size_t maxParamSize)
{
    regex_t regex;
    if (regcomp(&regex, "^(GET|POST|PUT|DELETE) /([^ ]*) HTTP/1", REG_EXTENDED) != 0)
    {
        regfree(&regex);
        return REGEX_COMPILE_FAILURE;
    }

    regmatch_t matches[3];
    if (regexec(&regex, buffer, 3, matches, 0) != 0)
    {
        regfree(&regex);
        return REGEX_NO_MATCH;
    }
    if (matches[1].rm_eo > maxMethodSize)
    {
        regfree(&regex);
        return REGEX_INVALID_METHOD;
    }

    extractMethod(buffer, matches[1].rm_eo, method, maxMethodSize);
    extractPath(buffer, matches[2].rm_eo, matches[2].rm_so, path);

    char* questionMarkLocation = strchr(path, '?' );
    if (questionMarkLocation)
    {
        long indexOfQuestionMark = questionMarkLocation-path;
        extractParams(path, indexOfQuestionMark, matches[2].rm_eo - indexOfQuestionMark, maxParamSize, params);
    }

    regfree(&regex);
    return REGEX_FIND_OK;
}

int getStringUpTo(char** str, char find, char* output, int maxOutput)
{
    char *next = strchr(*str, find);
    if (next == NULL)
    {
        return -1;
    }
    ++(*next);
    int end = (int)(next - *str);
    if (end > maxOutput)
    {
        return -1;
    }
    strncpy(output, *str, end);
    *str = next;
    return 0;
}

int extractHeadersAndBody(const char* buffer, kvpairs *headers, char* body, size_t maxBodySize, size_t maxHeaderSize)
{
    char *loc = strchr(buffer, '\n');
    ++loc;
    while (loc)
    {
        if (*loc == '\0' || (int)*loc == 13)
        {
            break;
        }
        char *head = malloc(sizeof(char) * maxHeaderSize);

        int res = getStringUpTo(&loc, ':', head, maxHeaderSize);
        if (res < 0) {
            free(head);
            break;
        }
        ++loc;
        while (*loc == ' ') ++loc;
        char *value = malloc(sizeof(char) * maxHeaderSize);
        res = getStringUpTo(&loc, '\n', value, maxHeaderSize);
        if (res < 0)
        {
            free(head);
            free(value);
            return -1;
        }
        ++loc;
        kvpair *newHeader = malloc(sizeof(kvpair));
        newHeader->name = head;
        newHeader->value = value;
        vector_pushBack(headers, (kvpair*)newHeader);
    }
    while (*loc != '\0' && ((int)*loc == 13 || (int)*loc == 10))
    {
        ++loc;
    }

    if (*loc != '\0')
    {
        size_t bodySize = strlen(loc);
        if (bodySize >= maxBodySize)
        {
            return BODY_SIZE_TOO_LARGE;
        }
        strncpy(body, loc, bodySize);
        body[bodySize] = '\0';
    }
    return 0;
}