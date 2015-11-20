#ifndef __SSAO_H
#define __SSAO_H

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <Eigen/Dense>
#include <types.h>
#include <vector>

#include <Gbuffer.hpp>

class SSAO {
    Gbuffer *gbuffer;
public:
    GLuint ssaoFBO;
    GLuint blurFBO;
    GLuint ssaoBuffer;
    GLuint blurBuffer;
    GLuint noiseTexture;
    SSAO(Gbuffer *gbuffer);
    void init(uint width, uint height);
    void generateNoiseTexture(const std::vector<Eigen::Vector3f> &noise);
    void bindOcclusionStage();

    void debugCopySSAO(uint width, uint height);
};

#endif
