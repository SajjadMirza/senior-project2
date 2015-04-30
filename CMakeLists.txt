# top-level

cmake_minimum_required (VERSION 2.8)
project (Tutorial)


include_directories(include)
include_directories(lib/include)

find_library(FMOD NAMES fmod fmodL PATHS lib/bin/fmod)
find_library(ASSIMP NAMES assimp PATHS lib/bin/assimp)

add_subdirectory(glfw)
include_directories(glfw/include)


add_subdirectory(src)





