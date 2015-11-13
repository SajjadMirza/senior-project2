#include <draw/ShapeBatch.hpp>
#include <log.hpp>


namespace draw {

    void ShapeBatch::init(GLuint matrix_attr)
    {
        h_trans = matrix_attr;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &trans_vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, trans_vbo);
        glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(mat4), &transforms[0], GL_STATIC_DRAW);

        GLsizei vec4size = sizeof(vec4);
        glEnableVertexAttribArray(matrix_attr);
        glVertexAttribPointer(matrix_attr, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)0);

        glEnableVertexAttribArray(matrix_attr + 1);
        glVertexAttribPointer(matrix_attr + 1, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, 
                              (GLvoid*)vec4size);

        glEnableVertexAttribArray(matrix_attr+2);
        glVertexAttribPointer(matrix_attr+2, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, 
                              (GLvoid*)(2*vec4size));

        glEnableVertexAttribArray(matrix_attr+3);
        glVertexAttribPointer(matrix_attr+3, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, 
                              (GLvoid*)(3*vec4size));

        glVertexAttribDivisor(matrix_attr+0, 1);
        glVertexAttribDivisor(matrix_attr+1, 1);
        glVertexAttribDivisor(matrix_attr+2, 1);
        glVertexAttribDivisor(matrix_attr+3, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void ShapeBatch::addTransform(const Eigen::Matrix4f &trans)
    {
        transforms.push_back(trans);
    }


    void ShapeBatch::drawAll(Program *prog)
    {
        target_shape->instanced_draw(trans_vbo, transforms.size(), prog, vao);
    }

    void ShapeBatch::drawAllDepth(Program *prog)
    {
        LOG("calls draw all depth");
        target_shape->instanced_draw_depth(trans_vbo, transforms.size(), prog, vao);
    }

};
