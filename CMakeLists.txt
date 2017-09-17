cmake_minimum_required (VERSION 3.1)
project(OpenCLMandelbrot)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake_files)
set( SDL2_PREFIX ${CMAKE_SOURCE_DIR}/)
set( SDL2_ROOT_DIR "SDL2-2.0.5")
find_package(OpenCL REQUIRED)
find_package(SDL2 REQUIRED)

find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
find_library(SDL2_LIBRARY NAME SDL2)

include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})
file(GLOB sources source/*.cpp source/*.h)
add_executable (main ${sources})

target_include_directories (main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SDL2_INCLUDE_DIR})
target_link_libraries (main ${OpenCL_LIBRARY} ${SDL2_LIBRARY} ${SDL2_SDLMAIN_LIBRARY})
