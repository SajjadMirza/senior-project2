# top-level

cmake_minimum_required (VERSION 2.8)
project (Tutorial)

# variables
set(GCC_CXX_STANDARD_LEVEL "-std=c++0x")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CXX_STANDARD_LEVEL}")

include_directories(include)
include_directories(lib/include)

find_library(FMOD NAMES fmod fmodL PATHS lib/bin/fmod)
find_library(ASSIMP NAMES assimp PATHS lib/bin/assimp)

add_subdirectory(glfw)
include_directories(glfw/include)


add_subdirectory(src)





