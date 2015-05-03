#include <draw/Drawable.hpp>

#include <stack>
#include <utility>

using draw::Node;

void assimp_to_eigen_matrix(Eigen::Matrix4f *dst, const aiMatrix4x4 &src) {
   Eigen::Matrix4f &m = *dst;

   /* assimp matrices are row-major so we need to do this backwards */

   m(0,0) = src.a1;
   m(1,0) = src.a2;
   m(2,0) = src.a3;
   m(3,0) = src.a4;
   m(0,1) = src.b1;
   m(1,1) = src.b2;
   m(2,1) = src.b3;
   m(3,1) = src.b4;
   m(0,2) = src.c1;
   m(1,2) = src.c2;
   m(2,2) = src.c3;
   m(3,2) = src.c4;
   m(0,3) = src.d1;
   m(1,3) = src.d2;
   m(2,3) = src.d3;
   m(3,3) = src.d4; // assimp sucks
   
}

Node *deepcopy_assimp_tree(const aiNode *node, const aiScene *scene) {
   Node *n = new Node();

   // copy transformation matrix
   assimp_to_eigen_matrix(&(n->transform), node->mTransformation);

   // copy meshes
   n->meshes.reserve(node->mNumMeshes);
   for (int i = 0; i < node->mNumMeshes; i++) {
      draw::Shape s;
      aiMesh *mesh = scene->mMeshes[i];
      s.init(*mesh, *scene);
      n->meshes.push_back(s);
   }

   // recurse
   n->children.reserve(node->mNumChildren);
   for (int i = 0; i < node->mNumChildren; i++) {
      aiNode *child = node->mChildren[i];
      Node *c = deepcopy_assimp_tree(child, scene);
      n->children.push_back(c);
   }

   return n;

}

namespace draw {
   Drawable::Drawable(const aiScene *scene) {
      /* copy the aiScene scene tree into our own scene graph */
      aiNode *rootAiNode = scene->mRootNode;
      root = deepcopy_assimp_tree(rootAiNode, scene);
   }

   Drawable::~Drawable() {}
};
