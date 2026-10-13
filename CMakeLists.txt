cmake_minimum_required(VERSION 3.16)

project(CountLines
VERSION 1.0.0
DESCRIPTION "Comando de contar linhas de arquivos"
LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(PROJECT_SOURCES
src/cl.c
src/hashset.c
)

set(PROJECT_HEADERS
include/hashset.h
)

add_executable(cl
${PROJECT_SOURCES}
${PROJECT_HEADERS}
)

target_include_directories(cl PRIVATE
"${CMAKE_CURRENT_SOURCE_DIR}/include"
)