#include <draw/Shape.hpp>

namespace draw {


    void flatten_array(std::vector<float> *dst, const aiVector3D *src, std::size_t num) {
        dst->reserve(3 * num);
        for (int i = 0; i < num; i++) {
            const aiVector3D v = src[i];
            dst->push_back(v.x);
            dst->push_back(v.y);
            dst->push_back(v.z);
        }
    }

    void flatten_indices(std::vector<unsigned int> *dst, const aiFace *src, std::size_t num) {
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

    Shape::Shape() : ver_buf(0), nor_buf(0), tex_buf(0), ind_buf(0) {}

    Shape::~Shape() {}

    void Shape::init(const TexTable &textures, const aiMesh& mesh, const aiScene& scene) {
        std::cout << "num textures in scene: " << scene.mNumTextures << std::endl;

        flatten_array(&vertices, mesh.mVertices, mesh.mNumVertices);

        glGenBuffers(1, &ver_buf);
        glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);


        flatten_array(&normals, mesh.mNormals, mesh.mNumVertices);

        glGenBuffers(1, &nor_buf);
        glBindBuffer(GL_ARRAY_BUFFER, nor_buf);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);

        // textures
        // TODO DON'T FORGET UVs
        aiMaterial *mat = scene.mMaterials[mesh.mMaterialIndex];
        aiString filename;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &filename);
        std::string name(filename.C_Str());
        
        tex_buf = textures.at(name).tid;

        flatten_indices(&indices, mesh.mFaces, mesh.mNumFaces);

        glGenBuffers(1, &ind_buf);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        assert(glGetError() == GL_NO_ERROR);
 
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
    
    	// TODO: Enable and bind texcoord array (if it exists) for drawing
    
    	// Bind index array for drawing
        int nIndices = indices.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
    
        // Draw
	    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
    
        // Disable and unbind

        // TODO: Disable Texture Buffer
    
	    GLSL::disableVertexAttribArray(h_nor);
	    GLSL::disableVertexAttribArray(h_vert);
	    glBindBuffer(GL_ARRAY_BUFFER, 0);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

}
