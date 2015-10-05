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
        GLuint tex_id_norm;

        GLuint tan_buf;
        GLuint bitan_buf;

    public:
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<uint> indices;
        std::vector<float> uvs;

        std::vector<float> tangents;
        std::vector<float> bitangents;
       
        Shape();
        ~Shape();

        void init(const TextureBundle &textures, const aiMesh &mesh);

        void colorDraw(int h_vert) const;
        void draw(int h_vert, int h_nor) const;
        void draw(int h_vert, int h_nor, int h_uv, int u_diffuse) const;
        void draw(int h_vert, int h_nor, int h_uv, int u_diffuse, int u_norm) const;
        void draw(int h_vert, int h_nor, int h_uv, int u_diffuse, int u_norm,
                  int h_tan, int h_btan) const;
    };

}

#endif
