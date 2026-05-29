cmake_minimum_required(VERSION 3.18)
project(GLIV)

# lib
add_library(gliv STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gliv.c
)

target_include_directories(gliv PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/examples
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/fonts
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/images
)

# ----------- examples -----------
# pages
add_subdirectory(submodules/minifb)

add_executable(gliv_windows_minifb_example examples/pages.c)

target_link_libraries(gliv_windows_minifb_example PRIVATE
    gliv
    minifb)

target_include_directories(gliv_windows_minifb_example PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/source
)

# pbm
add_executable(gliv_windows_pbm_export_example examples/pbm.c)
target_link_libraries(gliv_windows_pbm_export_example PRIVATE gliv)
target_include_directories(gliv_windows_pbm_export_example PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/source
)