#ifndef _SHADOW_MAP_HPP
#define _SHADOW_MAP_HPP

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include <Eigen/Dense>
#include <vector>
#include <Program.hpp>

namespace draw {
    class ShadowMap {
    public:
        GLuint cubemap;
        GLuint fbo;
        std::vector<Eigen::Matrix4f> transforms;
        GLuint sw, sh;
        void allocateBuffers(GLuint width, GLuint height);
      };
};

#endif
