#ifndef __SSAO_H
#define __SSAO_H

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <Eigen/Dense>
#include <types.h>
#include <vector>

class SSAO {
    GLuint colorFBO;
    GLuint blurFBO;
    GLuint colorBuffer;
    GLuint blurBuffer;
    GLuint noiseTexture;
public:
    void init(uint width, uint height);
    void generateNoiseTexture(const std::vector<Eigen::Vector3f> &noise);
};

#endif
