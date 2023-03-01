cmake_minimum_required(VERSION 3.19)

project(CMSC701_Project1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(sais src/libsais64.c src/libsais.c)
target_include_directories(sais PUBLIC include)

add_executable(buildsa src/buildsa.cpp)
target_link_libraries(buildsa PUBLIC sais)

add_executable(querysa src/querysa.cpp)
