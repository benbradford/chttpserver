cmake_minimum_required(VERSION 3.27)
project(untitled C)

set(INC ./inc)
set(SRC ./src)
set(CMAKE_C_STANDARD 23)

include_directories(${INC})

add_executable(untitled main.c
        ${SRC}/http/httpmethod.c
        inc/util/common.h
        inc/util/hash.h
        inc/util/stringbuilder.h
        inc/http/httprequest.h
        inc/http/httpheader.h
        inc/collection/vector.h
        inc/server/server.h
        inc/http/httpresponse.h
        inc/server/requestparser.h
        src/http/httprequest.c
        src/http/httpresponse.c
        src/server/requestparser.c
        src/server/server.c
        src/collection/vector.c
        src/util/stringbuilder.c
)
