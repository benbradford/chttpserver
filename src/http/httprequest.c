//
// Created by Bradford, Ben on 18/12/2023.
//
#include <util/common.h>
#include <http/httprequest.h>
#include <regex.h>

int httpRequest_findParameterValue(const httpRequest* request, char *name, char *value, int maxSize)
{
    if (!request)
    {
        return -1;
    }
    regex_t regex;
    char *regstring = malloc((sizeof(request->params) + sizeof(name) + 1) * sizeof(char));
    strcpy(regstring, name);
    strcat(regstring, "=(.*)$");
    int res = regcomp(&regex, regstring, REG_EXTENDED);
    int found = -1;
    if (res == 0)
    {
        int matchno = 2;
        regmatch_t matches[matchno];
        res = regexec(&regex, request->params, matchno, matches, 0);
        if (res == 0 && matches[1].rm_so != -1)
        {
            int endOfPath = matches[1].rm_eo - matches[1].rm_so;
            if (endOfPath > maxSize)
            {
                goto end;
            }
            strncpy(value, request->params + matches[1].rm_so, endOfPath);
            char *tok = strchr(value, '&');
            if (tok != NULL)
            {
                value[(int)(tok - value)] = '\0';
            }
            found = 0;
        }
    }
    end:
    free(regstring);
    return found;
}