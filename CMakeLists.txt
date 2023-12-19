cmake_minimum_required(VERSION 3.27)
project(httpserver C)

set(INC ./inc)
set(SRC ./src)
set(CMAKE_C_STANDARD 23)

include_directories(${INC})

add_executable(httpserver main.c
        ${SRC}/http/httpmethod.c
        inc/util/common.h
        inc/util/hash.h
        inc/util/stringbuilder.h
        inc/http/httprequest.h
        inc/collection/vector.h
        inc/server/server.h
        inc/http/httpresponse.h
        inc/server/requestparser.h
        src/http/httpresponse.c
        src/server/requestparser.c
        src/server/server.c
        src/collection/vector.c
        src/util/stringbuilder.c
        inc/util/kvpair.h
        src/util/kvpair.c
        inc/server/serverfunction.h
)
