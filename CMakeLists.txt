cmake_minimum_required(VERSION 3.18)
project(GLIV)

add_library(gliv STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gliv.c
)

target_include_directories(gliv PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/source
)