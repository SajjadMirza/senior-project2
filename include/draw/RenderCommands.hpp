#ifndef RENDER_COMMANDS_HPP
#define RENDER_COMMANDS_HPP

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

namespace draw {

    class Quad {
        GLuint vao;
        GLuint vbo;
    public:
        void GenerateData(int h_vert, int h_uv);
        void Render();
    };

};


#endif
