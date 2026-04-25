cmake_minimum_required(VERSION 3.18)
project(GLIV)

# lib
add_library(gliv STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gliv.c
)

target_include_directories(gliv PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/source
)

# example

add_subdirectory(submodules/minifb)

add_executable(gliv_windows_minifb_example example.c)

target_link_libraries(gliv_windows_minifb_example PRIVATE
    gliv
    minifb)

target_include_directories(gliv_windows_minifb_example PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/source
)