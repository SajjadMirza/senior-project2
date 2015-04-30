#include <Shape.hpp>

void flatten_array(std::vector<float> *dst, const aiVector3D *src, size_t num) {
   dst->reserve(3 * num);
   for (int i = 0; i < num; i++) {
      const aiVector3D v = src[i];
      dst->push_back(v.x);
      dst->push_back(v.y);
      dst->push_back(v.z);
   }
}

void flatten_indices(std::vector<unsigned int> *dst, const aiFace *src, size_t num_faces) {
   dst->reserve(3 * num);
   for (int i = 0; i < num; i++) {
      const aiFace f = src[i];
      dst->push_back(f[0]);
      dst->push_back(f[1]);
      dst->push_back(f[2]);
   }
}

Shape::Shape() : posBufID(0), norBufID(0), texBufID(0), indBuf(0) {
}

Shape::~Shape() {
}

void Shape::init(const aiMesh& mesh) {
   std::vector<float> vertices;
   flatten_array(&vertices, aiMesh.mVertices, aiMesh.mNumVertices);

   glGenBuffers(1, &ver_buf);
   glBindBuffer(GL_ARRAY_BUFFER, ver_buf);
   glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

   std::vector<float> normals;
   flatten_array(&normals, aiMesh.mNormals, aiMesh.mNumVertices);

   glGenBuffer(1, &nor_buf);
   glBindBuffer(GL_ARRAY_BUFFER, nor_buf);
   glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);

   // TODO: put textures here

   std::vector<unsigned int> indices;
   flatten_indices(&indices, aiMesh.mFaces, aiMesh.mNumFaces);

   glGenBuffers(1, &ind_buf);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   assert(glGetError() == GL_NO_ERROR);
}
