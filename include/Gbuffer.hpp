#ifndef _GBUFFER_H
#define _GBUFFER_H

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <types.h>

class Gbuffer {
    // handle to Frame Buffer Object
    GLuint fbo; 
    // handle to individual buffers
    GLuint gpos;
    GLuint gnor;
    GLuint gcol;
    GLuint gspc;
    GLuint zbuf;
    // attachment points
    GLuint attachments[4]; // initialized in init
public:

    bool init(uint width, uint height);
    void bind();
    void unbind();
    void bindTextures();
    void unbindTextures();
    void copyDepthBuffer(uint width, uint height);
};

#endif
