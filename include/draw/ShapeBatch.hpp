#ifndef _SHAPEBATCH_H
#define _SHAPEBATCH_H

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <draw/Shape.hpp>
#include <Eigen/Dense>
#include <Program.hpp>

namespace draw {

    class ShapeBatch {
        std::vector<Eigen::Matrix4f> transforms;
        Eigen::Vector3f offset;
    public:
        GLuint vao;
        GLuint trans_vbo;
        GLuint h_trans;
        const Shape *target_shape;


        void init(GLuint matrix_attribute);
        void addTransform(const Eigen::Matrix4f &trans);
        void drawAll(Program *prog);
        void drawAllDepth(Program *prog);
        void applyOffset(Eigen::Vector3f offset);
    };
    
};

#endif
