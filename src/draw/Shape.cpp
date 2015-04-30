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
         dst->push_back(f.mIndices[0]);
         dst->push_back(f.mIndices[1]);
         dst->push_back(f.mIndices[2]);
      }
   }

   Shape::Shape() : ver_buf(0), nor_buf(0), tex_buf(0), ind_buf(0) {}

   Shape::~Shape() {}

   void Shape::init(const aiMesh& mesh) {
      std::vector<float> vertices;
      flatten_array(&vertices, mesh.mVertices, mesh.mNumVertices);

      glGenBuffers(1, &ver_buf);
      glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
      glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

      std::vector<float> normals;
      flatten_array(&normals, mesh.mNormals, mesh.mNumVertices);

      glGenBuffers(1, &nor_buf);
      glBindBuffer(GL_ARRAY_BUFFER, nor_buf);
      glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);

      // TODO: put textures here

      std::vector<unsigned int> indices;
      flatten_indices(&indices, mesh.mFaces, mesh.mNumFaces);

      glGenBuffers(1, &ind_buf);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      assert(glGetError() == GL_NO_ERROR);
   }

}
