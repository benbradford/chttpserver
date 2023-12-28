//
// Created by Bradford, Ben on 21/12/2023.
//
#include "example/cars.h"
#include "thirdparty/cJSON.h"
#include "http/httpresponse.h"
#include "util/stringbuilder.h"

static const int maxResponseSize = 104857600;
static const int MAX_FILE_SIZE = 104857600;
const char * carsFile = "../resource/cars.json";

vector cars; // :TODO: Not thread safe

int addPairsArrayToVector(cJSON *arrayNode, const char* expectedName, const char *expectedValue, kvpairs *pairs, int (*shouldAddFunc)(const char*, const char*));
void outputCars();
char *createCarsBody();
void saveCars();
int deleteCar(const char *make, const char *model);
int canAddCar(const char *make, const char *model);
int canDeleteCar(const char *make, const char *model);

int cars_init()
{
    int res = vector_init(&cars, 32);
    if (res < 0)
    {
        return res;
    }
    FILE *file = fopen(carsFile, "r");
    if (!file)
    {
        return -3;
    }
    char *buffer = calloc(MAX_FILE_SIZE, sizeof(char));

    size_t read_chars = fread(buffer, sizeof(char), MAX_FILE_SIZE, file);
    if (read_chars <=0)
    {
        free(buffer);
        return -2;
    }
    buffer[read_chars] = '\0';

    fclose(file);

    cJSON *json = cJSON_Parse(buffer);
    int addPairsRes = addPairsArrayToVector(json->child->child, "make", "model", &cars, canAddCar);
    if (addPairsRes < 0)
    {
        return -4;
    }

    free(buffer);
    outputCars();
    return 0;
}

void cars_free()
{
    for (int i = 0; i < cars.size; ++i)
    {
        free(vector_get(&cars, i));
    }
    vector_free(&cars);
}

int cars_add(HttpRequest *req, HttpResponse * response)
{
    vector newCars;
    int result;
    cJSON *json = NULL;

    if (vector_init(&newCars, 8) < 0)
    {
        result = httpResponse_createError(500, "Unable to allocate", response);
        goto clean;
    }

    if (!req->body)
    {
        result = httpResponse_createError(400, "Missing Body", response);
        goto clean;
    }

    json = cJSON_Parse(req->body);
    if (!json || !json->child)
    {
        result = httpResponse_createError(400, "Invalid Json", response);
        goto clean;
    }
    int addPairsResult = addPairsArrayToVector(json->child->child, "make", "model", &newCars, canAddCar);
    if (addPairsResult != 0)
    {
        if (addPairsResult == -1 || addPairsResult == -2)
        {
            result = httpResponse_createError(400, "Unable to read cars array", response);
        }
        else
        {
            result = httpResponse_createError(400, "Duplicate Car present", response);
        }

        goto clean;
    }

    vector_addAll(&cars, &newCars);

    outputCars();


    result = httpResponse_create("HTTP/1.1 200 Success",
                               "200 Success",
                               NULL,
                               CONTENT_TYPE_JSON,
                               response);
    saveCars();
    clean:
    vector_free(&newCars);
    cJSON_Delete(json);
    return result;
}

int cars_get(HttpRequest *req __attribute__((unused)), HttpResponse * response)
{
    char *body = createCarsBody();

    int result = httpResponse_create("HTTP/1.1 200 Success",
                               body,
                               NULL,
                               CONTENT_TYPE_JSON,
                               response);
    free(body);
    return result;
}

int cars_delete(HttpRequest *req, HttpResponse * response)
{

    vector newCars;
    int result;
    cJSON *json = NULL;

    if (vector_init(&newCars, 8) < 0)
    {
        result = httpResponse_createError(500, "Unable to allocate", response);
        goto clean;
    }

    if (!req->body)
    {
        result = httpResponse_createError(400, "Missing Body", response);
        goto clean;
    }

    json = cJSON_Parse(req->body);
    if (!json || !json->child)
    {
        result = httpResponse_createError(400, "Invalid Json", response);
        goto clean;
    }
    int addPairsResult = addPairsArrayToVector(json->child->child, "make", "model", &newCars, canDeleteCar);
    if (addPairsResult != 0)
    {
        if (addPairsResult == -1 || addPairsResult == -2)
        {
            result = httpResponse_createError(400, "Unable to read cars array", response);
        }
        else
        {
            result = httpResponse_createError(400, "Car not present", response);
        }

        goto clean;
    }

    for (int i = 0; i < newCars.size; ++i)
    {
        kvpair *car = vector_get(&newCars, i);
        deleteCar(car->name, car->value);
    }

    outputCars();

    result = httpResponse_create("HTTP/1.1 200 Success",
                               "200 Success",
                               NULL,
                               CONTENT_TYPE_JSON,
                               response);
    saveCars();
    clean:
    vector_free(&newCars);
    cJSON_Delete(json);
    return result;
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
    char *response = calloc(strlen(sb.buffer), sizeof(char));
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
    printf("------cars------\n");
    for (int i = 0; i < cars.size; ++i)
    {
        kvpair *car = vector_get(&cars, i);
        printf("%s %s\n", car->name, car->value);
    }
    printf("----------------\n\n");
}

int canAddCar(const char *make, const char *model)
{
    for (int i = 0; i < cars.size; ++i)
    {
        kvpair *car = vector_get(&cars, i);
        if (strcmp(make, car->name) == 0 && strcmp(model, car->value) == 0) {
            return -1;
        }
    }
    return 0;
}

int canDeleteCar(const char *make, const char *model)
{
    for (int i = 0; i < cars.size; ++i)
    {
        kvpair *car = vector_get(&cars, i);
        if (strcmp(make, car->name) == 0 && strcmp(model, car->value) == 0) {
            return 0;
        }
    }
    return -1;
}

int deleteCar(const char *make, const char *model)
{
    for (int i = 0; i < cars.size; ++i) {
        kvpair *car = vector_get(&cars, i);
        if (strcmp(make, car->name) == 0 && strcmp(model, car->value) == 0) {
            vector_delete(&cars, i);
            return 0;
        }
    }
    return -1;
}

int addPairsArrayToVector(cJSON *arrayNode, const char* expectedName, const char *expectedValue, kvpairs *pairs, int (*shouldAddFunc)(const char*, const char*))
{
    cJSON *root = arrayNode;

    while (root) {
        cJSON *node = root->child;
        if (!node || strcmp(node->string, expectedName) < 0) return -1;
        char *first = node->valuestring;
        node = node->next;
        if (!node|| strcmp(node->string, expectedValue) < 0) return -2;
        char *second = node->valuestring;
        int canAddResult = shouldAddFunc(first, second);
        if (canAddResult < 0) {
            return -3;
        }
        kvpair *pair = calloc(1, sizeof(kvpair));
        char *name = calloc(strlen(first), sizeof(char));
        char *value = calloc(strlen(first), sizeof(char));
        strcpy(name, first);
        strcpy(value, second);
        pair->name = name;
        pair->value = value;
        vector_pushBack(pairs, pair);
        root = root->next;
    }
    return 0;
}