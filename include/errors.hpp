#ifndef _ERRORS_HPP
#define _ERRORS_HPP

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <log.hpp>

inline bool check_gl_errors()
{
    GLenum err = GL_NO_ERROR;
    bool error_occured = false;
    do {
        err = glGetError();
        error_occured = error_occured || (err != GL_NO_ERROR);
        switch (err) {
        case GL_NO_ERROR:
            break;
        case GL_INVALID_ENUM:
            ERROR("GL_INVALID_ENUM");
            break;
        case GL_INVALID_VALUE:
            ERROR("GL_INVALID_VALUE");
            break;
        case GL_INVALID_OPERATION:
            ERROR("GL_INVALID_OPERATION");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            ERROR("GL_INVALID_FRAMEBUFFER_OPERATION");
            break;
        case GL_OUT_OF_MEMORY:
            ERROR("GL_OUT_OF_MEMORY");
            break;
        case GL_STACK_UNDERFLOW:
            ERROR("GL_STACK_UNDERFLOW");
            break;
        case GL_STACK_OVERFLOW:
            ERROR("GL_STACK_OVERFLOW");
            break;
        }
    } while (err != GL_NO_ERROR);

    return error_occured;
}

#define CHECK_GL_ERRORS() assert(!check_gl_errors())

#endif
