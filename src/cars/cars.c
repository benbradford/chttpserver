//
// Created by Bradford, Ben on 21/12/2023.
//
#include "cars/cars.h"
#include "thirdparty/cJSON.h"
#include "collection/vector.h"
#include "util/kvpair.h"
#include "http/httpresponse.h"
#include "util/stringbuilder.h"

static const int maxResponseSize = 104857600;
static const int MAX_FILE_SIZE = 104857600;
const char * carsFile = "../resource/cars.json";

vector cars; // :TODO: Not thread safe

int addPairsArrayToVector(cJSON *arrayNode, const char* expectedName, const char *expectedValue, kvpairs *pairs);
void outputCars();
char *createCarsBody();
void saveCars();

int cars_init()
{
    int res = vector_init(&cars, 32);
    if (res < 0)
    {
        return res;
    }
    FILE *file = fopen(carsFile, "r");
    char *buffer = malloc(sizeof(char) * MAX_FILE_SIZE);
    size_t read_chars = 0;

    size_t length = ftell(file);
    read_chars = fread(buffer, sizeof(char), MAX_FILE_SIZE, file);

    buffer[read_chars] = '\0';

    fclose(file);

    cJSON *json = cJSON_Parse(buffer);
    addPairsArrayToVector(json->child->child, "make", "model", &cars);

    free(buffer);
    outputCars();
}

int cars_free()
{
    for (int i = 0; i < cars.size; ++i)
    {
        free(vector_get(&cars, i));
    }
    return vector_free(&cars);
}

size_t cars_add(httpRequest *req, char *responseString)
{
    kvpairs headers;
    vector newCars;
    size_t size;
    cJSON *json = NULL;

    if (vector_init(&headers ,8) < 0 || vector_init(&newCars, 8) < 0)
    {
        size = httpResponse_createErrorRequestWithReason(500, "Unable to allocate", responseString, maxResponseSize);
        goto clean;
    }

    if (!req->body)
    {
        size = httpResponse_createErrorRequestWithReason(400, "Missing Body", responseString, maxResponseSize);
        goto clean;
    }

    json = cJSON_Parse(req->body);
    if (!json || !json->child)
    {
        size = httpResponse_createErrorRequestWithReason(400, "Invalid Json", responseString, maxResponseSize);
        goto clean;
    }

    if (addPairsArrayToVector(json->child->child, "make", "model", &newCars) != 0)
    {
        size = httpResponse_createErrorRequestWithReason(400, "Unable to read cars array", responseString, maxResponseSize);
        goto clean;
    }

    vector_addAll(&cars, &newCars);

    outputCars();

    size = httpResponse_create("HTTP/1.1 200 Success",
                               "200 Success",
                               &headers,
                               responseString, maxResponseSize);
    saveCars();
    clean:
    vector_free(&newCars);
    vector_free(&headers);
    cJSON_Delete(json);
    return size;
}

size_t cars_get(httpRequest *req, char *responseString)
{
    kvpairs headers;
    vector_init(&headers ,8);

    char *body = createCarsBody();

    size_t size = httpResponse_create("HTTP/1.1 200 Success",
                               body,
                               &headers,
                               responseString, maxResponseSize);
    vector_free(&headers);
    free(body);
    return size;
}

char *createCarsBody()
{
    stringbuilder sb;
    sb_init(&sb, "{");
    sb_newLine(&sb);
    sb_appendNewLine(&sb, " \"cars\":");
    sb_appendNewLine(&sb, "  [");
    for (int i = 0; i < cars.size; ++i)
    {
        kvpair *pair = vector_get(&cars, i);
        sb_appendNewLine(&sb, "   {");
        sb_append(&sb, "    \"make\": \"");
        sb_append(&sb, pair->name);
        sb_append(&sb, "\", \"model\": \"");
        sb_append(&sb, pair->value);
        sb_appendNewLine(&sb, "\"");
        sb_append(&sb, "   }");
        if (i != cars.size-1)
        {
            sb_appendNewLine(&sb, ",");
        } else
        {
            sb_newLine(&sb);
        }
    }

    sb_appendNewLine(&sb, "  ]");
    sb_appendNewLine(&sb, "}");
    char *response = malloc(sizeof(char) * strlen(sb.buffer));
    strcpy(response, sb.buffer);
    sb_free(&sb);
    return response;
}

void saveCars()
{
    char *body = createCarsBody();
    FILE *file = fopen(carsFile, "w");
    fprintf(file, "%s", body);
    fclose(file);
    free(body);
}

void outputCars()
{
    for (int i = 0; i < cars.size; ++i)
    {
        kvpair *car = vector_get(&cars, i);
        printf("%s %s\n", car->name, car->value);
    }
}

int addPairsArrayToVector(cJSON *arrayNode, const char* expectedName, const char *expectedValue, kvpairs *pairs)
{
    cJSON *root = arrayNode;

    while (root) {
        cJSON *node = root->child;
        if (!node || strcmp(node->string, expectedName) < 0) return -1;
        char *first = node->valuestring;
        node = node->next;
        if (!node|| strcmp(node->string, expectedValue) < 0) return -2;
        char *second = node->valuestring;

        kvpair *pair = malloc(sizeof(kvpair));
        char *name = malloc(sizeof(char) * strlen(first));
        char *value = malloc(sizeof(char) * strlen(first));
        strcpy(name, first);
        strcpy(value, second);
        pair->name = name;
        pair->value = value;
        vector_pushBack(pairs, pair);
        root = root->next;
    }
    return 0;
}