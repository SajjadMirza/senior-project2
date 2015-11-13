#include <draw/ShadowMap.hpp>

#include <log.hpp>

namespace draw {
    void ShadowMap::allocateBuffers(GLuint width, GLuint height)
    {
        sw = width;
        sh = height;
        glGenFramebuffers(1, &fbo);
        glGenTextures(1, &cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        for (int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                         sw, sh, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubemap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
            
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            ERROR("Incomplete framebuffer: " << fbo);
            exit(-1);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};
