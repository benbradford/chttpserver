//
// Created by Bradford, Ben on 18/12/2023.
//
#include <server/requestparser.h>
#include <http/httpheader.h>
#include <http/httpmethod.h>
#include <collection/vector.h>

#include <regex.h>

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

int extractMethodPathAndParam(const char* buffer, int* method, char *path, char *params, int maxMethodSize)
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
                *method = httpMethodFromString(methodString);
                int endOfPath = matches[2].rm_eo - matches[2].rm_so;
                strncpy(path, buffer + matches[2].rm_so, endOfPath);
                path[endOfPath] = '\0';
                url_decode(path);
                char* loc = strchr(path, '?' );
                status = REGEX_FIND_OK;
                if (loc == NULL)
                {
                    params = NULL;
                }
                else
                {
                    int locationOfQ = loc-path;
                    int paramsLen = matches[2].rm_eo - locationOfQ;
                    memcpy(params, path + locationOfQ + 1, paramsLen );
                    path[locationOfQ] = '\0';
                    params[paramsLen] = '\0';
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

    strncpy(output, *str, start);
    *str = next;
    return 0;
}

int extractHeadersAndBody(char* buffer, vector* headers, char* body, int maxBodySize)
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
        httpHeader *newHeader = malloc(sizeof(httpHeader));
        newHeader->name = head;
        newHeader->value = value;
        vector_pushBack(headers, (httpHeader*)newHeader);
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