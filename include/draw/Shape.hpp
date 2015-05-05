#ifndef _SHAPE_H
#define _SHAPE_H

#include <common.hpp>

#include <draw/Texture.hpp>

namespace draw {

    class Shape {
    private:
        GLuint ver_buf;
        GLuint nor_buf;
        GLuint ind_buf;
        GLuint uv_buf;

        GLuint tex_id_diffuse;

    public:
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<uint> indices;
        std::vector<float> uvs;
       
        Shape();
        ~Shape();
        void init(const TexTable &textures, const aiMesh& mesh, const aiScene& scene);
        void draw(int h_vert, int h_nor, int h_uv, int u_diffuse) const;
    };

}

#endif
