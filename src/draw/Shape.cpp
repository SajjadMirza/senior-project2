#include <draw/Shape.hpp>

#include <tuple>

#define LOG_THIS_FILE 0
#include <log.hpp>

#include <errors.hpp>

#define LOG_DRAW_CALLS 0



extern int special_texture_handle;

namespace draw {

    void Shape::instanced_draw_depth(GLuint matrix_buffer, int amount, Program *prog, GLuint vao)
        const
    {
        LOG("HELLO FROM Shape::instanced_draw_depth");
        GLuint h_vert = prog->getAttribute("vertPos");
        GLuint h_M = prog->getAttribute("iM");
        LOG("vertPos handle for depth draw: " << h_vert);
        LOG("iM handle for depth draw: " << h_M);
        
        glBindVertexArray(vao);
        
#if 1
        // Enable and bind verticies array for drawing
        GLSL::enableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
        glVertexAttribPointer(h_vert, 3, GL_FLOAT, GL_FALSE, 0, 0);
#if 1
        // Bind instanced model matrix array
        glBindBuffer(GL_ARRAY_BUFFER, matrix_buffer);
        // Enable the matrix array as multiple arrays of vec4
        GLsizei vec4size = sizeof(vec4);
        glEnableVertexAttribArray(h_M+0);
        glVertexAttribPointer(h_M+0, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)0);

        glEnableVertexAttribArray(h_M+1);
        glVertexAttribPointer(h_M+1, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)vec4size);
        glEnableVertexAttribArray(h_M+2);
        glVertexAttribPointer(h_M+2, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(2*vec4size));
        glEnableVertexAttribArray(h_M+3);
        glVertexAttribPointer(h_M+3, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(3*vec4size));
/**/
        // Enable divisors

        glVertexAttribDivisor(h_M+0, 1);
        glVertexAttribDivisor(h_M+1, 1);
        glVertexAttribDivisor(h_M+2, 1);
        glVertexAttribDivisor(h_M+3, 1);
#endif
#endif

        // Bind index array for drawing
        int nIndices = indices.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);

        // Draw
        glDrawElementsInstanced(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0, amount);

        glVertexAttribDivisor(h_M+0, 0);
        glVertexAttribDivisor(h_M+1, 0);
        glVertexAttribDivisor(h_M+2, 0);
        glVertexAttribDivisor(h_M+3, 0);

        GLSL::disableVertexAttribArray(h_M+3);
        GLSL::disableVertexAttribArray(h_M+2);
        GLSL::disableVertexAttribArray(h_M+1);
        GLSL::disableVertexAttribArray(h_M+0);
        GLSL::disableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    void Shape::instanced_draw(GLuint matrix_buffer, int amount, Program *prog, GLuint vao) const
    {
        GLuint h_vert = prog->getAttribute("vertPos");
        GLuint h_nor =  prog->getAttribute("vertNor");
        GLuint h_uv = prog->getAttribute("vertTex");
        GLuint u_diffuse = prog->getUniform("texture0");
        GLuint u_norm = prog->getUniform("texture_norm");
        GLuint u_specular = prog->getUniform("texture_spec");
        GLuint h_tan = prog->getAttribute("tangent");
        GLuint h_btan = prog->getAttribute("bitangent");
        GLuint h_M = prog->getAttribute("iM");

//        LOG("iM handle for gbuffer draw: " << h_M);

        glBindVertexArray(vao);
        
        // Enable norm texture
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, tex_id_norm);
        glUniform1i(u_norm, 0);

        // Enable diffuse texture
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, tex_id_diffuse);
        glUniform1i(u_diffuse, 1);

        // Enable specular texture
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, tex_id_specular);
        glUniform1i(u_specular, 2);
        
        // Enable and bind verticies array for drawing
        GLSL::enableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
        glVertexAttribPointer(h_vert, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Enable and bind normal array for drawing
        GLSL::enableVertexAttribArray(h_nor);
        glBindBuffer(GL_ARRAY_BUFFER, nor_buf);
        glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
        // TODO: Enable and bind texcoord array (if it exists) for drawing
        GLSL::enableVertexAttribArray(h_uv);
        glBindBuffer(GL_ARRAY_BUFFER, uv_buf);
        glVertexAttribPointer(h_uv, 2, GL_FLOAT, GL_FALSE, 0, 0);

        // Enable and bind tangent array for drawing
        GLSL::enableVertexAttribArray(h_tan);
        glBindBuffer(GL_ARRAY_BUFFER, tan_buf);
        glVertexAttribPointer(h_tan, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Enable and bind bitangent array for drawing
        GLSL::enableVertexAttribArray(h_btan);
        glBindBuffer(GL_ARRAY_BUFFER, bitan_buf);
        glVertexAttribPointer(h_btan, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Bind index array for drawing
        int nIndices = indices.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
/**/
        // Bind instanced model matrix array
        glBindBuffer(GL_ARRAY_BUFFER, matrix_buffer);
        // Enable the matrix array as multiple arrays of vec4
        GLsizei vec4size = sizeof(vec4);
        glEnableVertexAttribArray(h_M+0);
        glVertexAttribPointer(h_M+0, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)0);
        glEnableVertexAttribArray(h_M+1);
        glVertexAttribPointer(h_M+1, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)vec4size);
        glEnableVertexAttribArray(h_M+2);
        glVertexAttribPointer(h_M+2, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(2*vec4size));
        glEnableVertexAttribArray(h_M+3);
        glVertexAttribPointer(h_M+3, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (GLvoid*)(3*vec4size));
        // Enable divisors

        glVertexAttribDivisor(h_M+0, 1);
        glVertexAttribDivisor(h_M+1, 1);
        glVertexAttribDivisor(h_M+2, 1);
        glVertexAttribDivisor(h_M+3, 1);

/**/
//        glBindVertexArray(0);
        
        // Draw
        glDrawElementsInstanced(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0, amount);

        // Disable and unbind
        glBindTexture(GL_TEXTURE_2D, 0);


        glVertexAttribDivisor(h_M+0, 0);
        glVertexAttribDivisor(h_M+1, 0);
        glVertexAttribDivisor(h_M+2, 0);
        glVertexAttribDivisor(h_M+3, 0);

        GLSL::disableVertexAttribArray(h_M+3);
        GLSL::disableVertexAttribArray(h_M+2);
        GLSL::disableVertexAttribArray(h_M+1);
        GLSL::disableVertexAttribArray(h_M+0);
        GLSL::disableVertexAttribArray(h_btan);
        GLSL::disableVertexAttribArray(h_tan);
        GLSL::disableVertexAttribArray(h_uv);
        GLSL::disableVertexAttribArray(h_nor);
        GLSL::disableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }


    static void flatten_array(std::vector<float> *dst, const aiVector3D *src, std::size_t num) 
    {
        dst->reserve(3 * num);
        for (int i = 0; i < num; i++) {
            const aiVector3D v = src[i];
            dst->push_back(v.x);
            dst->push_back(v.y);
            dst->push_back(v.z);
        }
    }

    static void flatten_array2D(std::vector<float> *dst, const aiVector3D *src, std::size_t num) 
    {
        dst->reserve(2 * num);
        for (int i = 0; i < num; i++) {
            const aiVector3D &v = src[i];
            dst->push_back(v.x);
            dst->push_back(v.y);
        }
    }

    static void flatten_indices(std::vector<unsigned int> *dst, const aiFace *src, 
                                std::size_t num) 
    {
        dst->reserve(3 * num);
        for (int i = 0; i < num; i++) {
            const aiFace f = src[i];
            
            if (f.mNumIndices != 3) {
                std::cout << "face indices " << f.mNumIndices << std::endl;
            }
            
            dst->push_back(f.mIndices[0]);
            dst->push_back(f.mIndices[1]);
            dst->push_back(f.mIndices[2]);
        }
    }

    Shape::Shape() : ver_buf(0), nor_buf(0), tex_id_diffuse(0), tex_id_norm(0), ind_buf(0) {}

    Shape::~Shape() {}

    void Shape::init(const TextureBundle &textures, const aiMesh &mesh) {
        CHECK_GL_ERRORS();
        LOG("shape 1");
        // copy vertex coordinates
        flatten_array(&vertices, mesh.mVertices, mesh.mNumVertices);
        glGenBuffers(1, &ver_buf);
        glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
                     &vertices[0], GL_STATIC_DRAW);
        CHECK_GL_ERRORS();
        LOG("shape 2");
        // copy vertex normal vectors
        flatten_array(&normals, mesh.mNormals, mesh.mNumVertices);
        glGenBuffers(1, &nor_buf);
        glBindBuffer(GL_ARRAY_BUFFER, nor_buf);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float),
                     &normals[0], GL_STATIC_DRAW);
        CHECK_GL_ERRORS();
        // copy textures
        if (textures.diffuse) {
            tex_id_diffuse = textures.diffuse->tid;
        }

        if (textures.normal) {
            tex_id_norm = textures.normal->tid;
        }

        if (textures.specular) {
            tex_id_specular = textures.specular->tid;
        }

        LOG("shape 3");
        // copy uv coordinates
        CHECK_GL_ERRORS();
        flatten_array2D(&uvs, mesh.mTextureCoords[0], mesh.mNumVertices);
        LOG("shape 3.1");
        glGenBuffers(1, &uv_buf);
        LOG("shape 3.2");
        glBindBuffer(GL_ARRAY_BUFFER, uv_buf);
        LOG("shape 3.3");
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), &uvs[0],
                     GL_STATIC_DRAW);
        LOG("shape 3.4");

        LOG("shape 4");
        // copy indices
        flatten_indices(&indices, mesh.mFaces, mesh.mNumFaces);
        glGenBuffers(1, &ind_buf);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint),
                     &indices[0], GL_STATIC_DRAW);
        CHECK_GL_ERRORS();
        LOG("shape 5");
        if (mesh.HasTangentsAndBitangents()) {
            LOG("shape 5.1");
            // copy tangents
            flatten_array(&tangents, mesh.mTangents, mesh.mNumVertices);
            glGenBuffers(1, &tan_buf);
            glBindBuffer(GL_ARRAY_BUFFER, tan_buf);
            glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(float), &tangents[0],
                         GL_STATIC_DRAW);
            LOG("shape 5.2");
            // copy bitangents
            flatten_array(&bitangents, mesh.mBitangents, mesh.mNumVertices);
            glGenBuffers(1, &bitan_buf);
            glBindBuffer(GL_ARRAY_BUFFER, bitan_buf);
            glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(float), &bitangents[0],
                         GL_STATIC_DRAW);
            LOG("shape 5.3");
        }
        CHECK_GL_ERRORS();
        LOG("shape 6");
        // unbind buffers
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        CHECK_GL_ERRORS();
        LOG("shape 7");
        // check for errors
        CHECK_GL_ERRORS();
        assert(glGetError() == GL_NO_ERROR);
    }

    void Shape::colorDraw(int h_vert) const {
        // Enable and bind verticies array for drawing
        GLSL::enableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
        glVertexAttribPointer(h_vert, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Bind index array for drawing
        int nIndices = indices.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);

        // Draw
        glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

        // Disable and unbind
        GLSL::disableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    }

    void Shape::draw(int h_vert, int h_nor) const {
        // Enable and bind verticies array for drawing
        GLSL::enableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
        glVertexAttribPointer(h_vert, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
        // Enable and bind normal array for drawing
        GLSL::enableVertexAttribArray(h_nor);
        glBindBuffer(GL_ARRAY_BUFFER, nor_buf);
        glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Bind index array for drawing
        int nIndices = indices.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
    
        // Draw
        glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
    
        // Disable and unbind
        GLSL::disableVertexAttribArray(h_nor);
        GLSL::disableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    

    void Shape::draw(int h_vert, int h_nor, int h_uv, int u_diffuse) const {
#if LOG_DRAW_CALLS
        static int logged = 0;
        LOG("DRAW WITHOUT NORMAL MAP");
//        if (!logged) {
        LOG("h_vert " << h_vert << " h_nor " << h_nor << " h_uv " << h_uv << "u_diffuse "
            << u_diffuse);
        logged = 1;
//        }
#endif
        // Enable diffuse texture
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, tex_id_diffuse);
        glUniform1i(u_diffuse, 0);
        
    	// Enable and bind verticies array for drawing
        GLSL::enableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
        glVertexAttribPointer(h_vert, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
        // Enable and bind normal array for drawing
        GLSL::enableVertexAttribArray(h_nor);
        glBindBuffer(GL_ARRAY_BUFFER, nor_buf);
        glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    	// TODO: Enable and bind texcoord array (if it exists) for drawing
        GLSL::enableVertexAttribArray(h_uv);
        glBindBuffer(GL_ARRAY_BUFFER, uv_buf);
        glVertexAttribPointer(h_uv, 2, GL_FLOAT, GL_FALSE, 0, 0);


    	// Bind index array for drawing
        int nIndices = indices.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
    
        // Draw
        glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
    
        // Disable and unbind
        glBindTexture(GL_TEXTURE_2D, 0);
        GLSL::disableVertexAttribArray(h_uv);
        GLSL::disableVertexAttribArray(h_nor);
        GLSL::disableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }



    void Shape::draw(int h_vert, int h_nor, int h_uv, int u_diffuse, int u_norm) const {
#if LOG_DRAW_CALLS
        static int logged = 0;
//        if (!logged) {
        LOG("h_vert " << h_vert << " h_nor " << h_nor << " h_uv " << h_uv << " u_diffuse "
            << u_diffuse << " u_norm " << u_norm);
        logged = 1;
//        }
#endif

//        LOG("DRAW WITH NORMAL MAP");
        // Enable norm texture
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, tex_id_norm);
        glUniform1i(u_norm, 0);

        // Enable diffuse texture
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, tex_id_diffuse);
        glUniform1i(u_diffuse, 1);
        
        // Enable and bind verticies array for drawing
        GLSL::enableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
        glVertexAttribPointer(h_vert, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Enable and bind normal array for drawing
        GLSL::enableVertexAttribArray(h_nor);
        glBindBuffer(GL_ARRAY_BUFFER, nor_buf);
        glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
        // TODO: Enable and bind texcoord array (if it exists) for drawing
        GLSL::enableVertexAttribArray(h_uv);
        glBindBuffer(GL_ARRAY_BUFFER, uv_buf);
        glVertexAttribPointer(h_uv, 2, GL_FLOAT, GL_FALSE, 0, 0);


        // Bind index array for drawing
        int nIndices = indices.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
   
        // Draw
        glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

        // Disable and unbind
        glBindTexture(GL_TEXTURE_2D, 0);

        GLSL::disableVertexAttribArray(h_uv);
        GLSL::disableVertexAttribArray(h_nor);
        GLSL::disableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Shape::draw(int h_vert, int h_nor, int h_uv, int u_diffuse, int u_norm,
                     int u_specular, int h_tan, int h_btan) const
    {
#if LOG_DRAW_CALLS
        static int logged = 0;
//        if (!logged) {
        LOG("h_vert " << h_vert << " h_nor " << h_nor << " h_uv " << h_uv << " u_diffuse "
            << u_diffuse << " u_norm " << u_norm << " u_specular " << u_specular);
        logged = 1;
//        }
#endif

//        LOG("DRAW WITH NORMAL MAP");
        // Enable norm texture
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, tex_id_norm);
        glUniform1i(u_norm, 0);

        // Enable diffuse texture
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, tex_id_diffuse);
        glUniform1i(u_diffuse, 1);

        // Enable specular texture
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, tex_id_specular);
        glUniform1i(u_specular, 2);
        
        // Enable and bind verticies array for drawing
        GLSL::enableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
        glVertexAttribPointer(h_vert, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Enable and bind normal array for drawing
        GLSL::enableVertexAttribArray(h_nor);
        glBindBuffer(GL_ARRAY_BUFFER, nor_buf);
        glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
        // TODO: Enable and bind texcoord array (if it exists) for drawing
        GLSL::enableVertexAttribArray(h_uv);
        glBindBuffer(GL_ARRAY_BUFFER, uv_buf);
        glVertexAttribPointer(h_uv, 2, GL_FLOAT, GL_FALSE, 0, 0);

        // Enable and bind tangent array for drawing
        GLSL::enableVertexAttribArray(h_tan);
        glBindBuffer(GL_ARRAY_BUFFER, tan_buf);
        glVertexAttribPointer(h_tan, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Enable and bind bitangent array for drawing
        GLSL::enableVertexAttribArray(h_btan);
        glBindBuffer(GL_ARRAY_BUFFER, bitan_buf);
        glVertexAttribPointer(h_btan, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Bind index array for drawing
        int nIndices = indices.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
   
        // Draw
        glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

        // Disable and unbind
        glBindTexture(GL_TEXTURE_2D, 0);

        GLSL::disableVertexAttribArray(h_btan);
        GLSL::disableVertexAttribArray(h_tan);
        GLSL::disableVertexAttribArray(h_uv);
        GLSL::disableVertexAttribArray(h_nor);
        GLSL::disableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Shape::drawSpec(int h_vert, int h_nor, int h_uv, int u_diffuse, int u_spec) const
    {
#if LOG_DRAW_CALLS
        static int logged = 0;
        LOG("DRAW WITHOUT NORMAL MAP");
//        if (!logged) {
        LOG("h_vert " << h_vert << " h_nor " << h_nor << " h_uv " << h_uv << "u_diffuse "
            << u_diffuse << "u_spec" << u_spec);
        logged = 1;
//        }
#endif
        // Enable diffuse texture
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, tex_id_diffuse);
        glUniform1i(u_diffuse, 0);

        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, tex_id_specular);
        glUniform1i(u_spec, 1);
        
    	// Enable and bind verticies array for drawing
        GLSL::enableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
        glVertexAttribPointer(h_vert, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
        // Enable and bind normal array for drawing
        GLSL::enableVertexAttribArray(h_nor);
        glBindBuffer(GL_ARRAY_BUFFER, nor_buf);
        glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    	// TODO: Enable and bind texcoord array (if it exists) for drawing
        GLSL::enableVertexAttribArray(h_uv);
        glBindBuffer(GL_ARRAY_BUFFER, uv_buf);
        glVertexAttribPointer(h_uv, 2, GL_FLOAT, GL_FALSE, 0, 0);


    	// Bind index array for drawing
        int nIndices = indices.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
    
        // Draw
        glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
    
        // Disable and unbind
        glBindTexture(GL_TEXTURE_2D, 0);
        GLSL::disableVertexAttribArray(h_uv);
        GLSL::disableVertexAttribArray(h_nor);
        GLSL::disableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Shape::drawLightVolume(int h_vert) const
    {
        //LOG("drawAsLightVolume indices.size()" << indices.size());
        // Enable vertices array for drawing
        GLSL::enableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
        glVertexAttribPointer(h_vert, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Bind index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);

        // Draw
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // Disable and unbind
        GLSL::disableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Shape::drawDepth(int h_vert) const
    {
        GLSL::enableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
        glVertexAttribPointer(h_vert, 3, GL_FLOAT, GL_FALSE, 0, 0);
     
        // Bind index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);

        // Draw
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // Disable and unbind
        GLSL::disableVertexAttribArray(h_vert);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   
    }

}
