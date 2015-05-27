# top-level

cmake_minimum_required (VERSION 2.8)
project (Tutorial)

# variables
set(GCC_CXX_STANDARD_LEVEL "-std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CXX_STANDARD_LEVEL}")
set(INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/install")
set(CMAKE_INSTALL_PREFIX "${INSTALL_DIR}")
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

include_directories(include)
include_directories(lib/include)
include_directories(lib/include/freetype)

find_library(FMOD NAMES fmod fmodL PATHS lib/bin/fmod)
find_library(ASSIMP NAMES assimp PATHS lib/bin/assimp)
find_library(FREEIMG NAMES freeimageplus PATHS lib/bin/freeimageplus)
find_library(YAMLCPP NAMES yaml-cpp PATHS lib/bin/yaml-cpp)
find_library(FREETYPE NAMES freetype PATHS lib/bin/freetype)
set(EXTERNAL_LIBS ${FMOD} ${ASSIMP} ${FREEIMG} ${YAMLCPP} ${FREETYPE})

set(BOOST_ROOT .)
set(BOOST_INCLUDEDIR ./lib/include)
set(BOOST_LIBRARYDIR ./lib/bin/boost)
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_DEBUG OFF)
message("hello from boost")
find_package(Boost 1.50 COMPONENTS system filesystem REQUIRED)
message("hello from boost 2")
set(EXTERNAL_LIBS ${EXTERNAL_LIBS} ${Boost_LIBRARIES})

add_subdirectory(glfw)
include_directories(glfw/include)
set(EXTERNAL_LIBS ${EXTERNAL_LIBS} glfw ${GLFW_LIBRARIES})

install(DIRECTORY resources DESTINATION "${INSTALL_DIR}")
#install(FILES README.md DESTINATION bin)

add_subdirectory(src)







