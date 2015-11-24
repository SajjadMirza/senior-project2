#ifndef _COMMON_H
#define _COMMON_H
#define GL_GLEXT_PROTOTYPES

// GLFW
/* External headers. */

// GLFW and also OpenGL
#include <GLFW/glfw3.h>

// Eigen
#include <Eigen/Dense>

// ASSIMP
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

// FreeImage
#include <FreeImagePlus.h>

// C++ headers
#include <string>
#include <iostream>
#include <vector>

// C headers
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>

/* Global declarations. */

// constants
#include <constants.hpp>

// type declarations
#include <types.h>

/* Internal headers. */

// Sueda
#include <GLSL.hpp>
#include <Program.hpp>

#endif
