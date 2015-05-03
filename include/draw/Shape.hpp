#ifndef _SHAPE_H
#define _SHAPE_H

#include <common.hpp>

namespace draw {

    class Shape {
    private:
        GLuint ver_buf;
        GLuint nor_buf;
        GLuint tex_buf;
        GLuint ind_buf;

    public:
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<uint> indices;
       
        Shape();
        ~Shape();
        void init(const aiMesh& mesh);
        void draw(int h_vert, int h_nor) const;
    };

}

#endif
