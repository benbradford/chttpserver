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
            decoded[decoded_len++] = hex_val;
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
    int paramLength = strlen(param);
    regex_t regex;
    char *regstring = "^(.*)=(.*)$";
    int res = regcomp(&regex, regstring, REG_EXTENDED);
    if (res == 0)
    {
        int matchno = 3;
        regmatch_t matches[matchno];
        res = regexec(&regex, param, matchno, matches, 0);
        if (res == 0 && matches[1].rm_so != -1)
        {
            int endOfName = matches[1].rm_eo - matches[1].rm_so;
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
}

int extractMethodPathAndParam(const char* buffer, int* method, char *path, kvpairs *params, int maxMethodSize)
{
    regex_t regex;
    int res = regcomp(&regex, "^(GET|POST|PUT|DELETE) /([^ ]*) HTTP/1", REG_EXTENDED);
    int status = REGEX_NO_MATCH;
    if (res != 0)
    {
        status = REGEX_COMPILE_FAILURE;
    }
    else
    {
        int matchno = 3;
        regmatch_t matches[matchno];
        res = regexec(&regex, buffer, matchno, matches, 0);
        if (res == 0)
        {
            if (matches[1].rm_eo > maxMethodSize)
            {
                status = REGEX_INVALID_METHOD;
            }
            else
            {
                char *methodString = malloc(maxMethodSize * sizeof(char));
                strncpy(methodString, buffer, matches[1].rm_eo);
                methodString[matches[1].rm_eo] = '\0';
                *method = httpMethod_fromStrong(methodString);
                int endOfPath = matches[2].rm_eo - matches[2].rm_so;
                strncpy(path, buffer + matches[2].rm_so, endOfPath);
                path[endOfPath] = '\0';
                url_decode(path);
                char* loc = strchr(path, '?' );
                status = REGEX_FIND_OK;
                if (loc)
                {
                    const int maxParamLength = 256;
                    int locationOfQ = loc-path;
                    int paramsLen = matches[2].rm_eo - locationOfQ;
                    char *paramString = malloc(sizeof(char) * maxParamLength);
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
                            thisParam[(int)(nextParam - thisParam) - 1] = '\0';
                        }

                        kvpair *p = createParameter(thisParam);
                        if (p) {
                            vector_pushBack(params, p);
                        }
                        thisParam = nextParam;

                    }

                    free(paramString);
                }
                free(methodString);
            }
        }
    }
    regfree(&regex);
    return status;
}

int getStringUpTo(char** str, char find, char* output, int maxOutput)
{
    char *next = strchr(*str, find);
    if (next == NULL)
    {
        return -1;
    }
    ++(*next);
    int start = (int)(next - *str);
    // :TODO: return -1 if > maxOutput
    strncpy(output, *str, start);
    *str = next;
    return 0;
}

int extractHeadersAndBody(char* buffer, kvpairs *headers, char* body, int maxBodySize)
{
    char *loc = strchr(buffer, '\n');
    ++loc;
    while (1)
    {
        if (*loc == '\0' || (int)*loc == 13)
        {
            break;
        }
        char *head = malloc(sizeof(char) * 256);

        int res = getStringUpTo(&loc, ':', head, 256);
        if (res < 0) {
            free(head);
            break;
        }
        ++loc;
        while (*loc != '\0' && *loc == ' ')
        {
            ++loc;
        }
        char *value = malloc(sizeof(char) * 256);
        res = getStringUpTo(&loc, '\n', value, 256);
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
        int bodySize = strlen(loc);
        if (bodySize >= maxBodySize)
        {
            return BODY_SIZE_TOO_LARGE;
        }
        strncpy(body, loc, bodySize);
        body[bodySize] = '\0';
    }
    return 0;
}