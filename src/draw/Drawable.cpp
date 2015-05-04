#include <draw/Drawable.hpp>

#include <stack>
#include <utility>
#include <vector>

#include <resources.hpp>

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

Node *deepcopy_assimp_tree(const draw::Drawable *parent_drawable, const aiNode *node, const aiScene *scene) {
   Node *n = new Node();

   // copy transformation matrix
   assimp_to_eigen_matrix(&(n->transform), node->mTransformation);

   // copy meshes
   n->meshes.reserve(node->mNumMeshes);
   for (int i = 0; i < node->mNumMeshes; i++) {
      draw::Shape s;
      aiMesh *mesh = scene->mMeshes[i];
      s.init(parent_drawable->textures, *mesh, *scene);
      n->meshes.push_back(s);
   }

   // recurse
   n->children.reserve(node->mNumChildren);
   for (int i = 0; i < node->mNumChildren; i++) {
      aiNode *child = node->mChildren[i];
      Node *c = deepcopy_assimp_tree(parent_drawable, child, scene);
      n->children.push_back(c);
   }

   return n;

}

static const uint RGB_BITS = 24, RGBA_BITS = 32;
static const uint RGB_CHANNELS = 3, RGBA_CHANNELS = 4;

namespace draw {

    
    Drawable::Drawable(const aiScene *scene, std::string& dir) {
      /* copy the aiScene scene tree into our own scene graph */
      aiNode *rootAiNode = scene->mRootNode;

      // TODO move this loop into its own function
      for (int i = 0; i < scene->mNumMaterials; i++) {
          aiMaterial *mat = scene->mMaterials[i];
          aiString filename;
          mat->GetTexture(aiTextureType_DIFFUSE, 0, &filename);
          std::string name(filename.C_Str());
          std::string file_location = dir + "/" +  name;

          FIBITMAP *img = resource::GenericLoader(file_location.c_str(), 0);

          uint bit_depth = FreeImage_GetBPP(img);
          BYTE *bits = FreeImage_GetBits(img);

          Texture tex;
          tex.filename = name;
          tex.type = TexType::DIFFUSE;
    
          glGenTextures(1, &(tex.tid));

          glBindTexture(GL_TEXTURE_2D, tex.tid);

          int width = FreeImage_GetWidth(img);
          int height = FreeImage_GetHeight(img);

          if (bit_depth == RGB_BITS) {
              glTexImage2D(GL_TEXTURE_2D,
                           0,
                           RGB_CHANNELS,
                           width,
                           height,
                           0,
                           GL_RGB,
                           GL_UNSIGNED_BYTE,
                           (void*) bits); 
          }
          else if (bit_depth == RGBA_BITS) {
              glTexImage2D(GL_TEXTURE_2D,
                           0, RGBA_CHANNELS,
                           width,
                           height,
                           0,
                           GL_RGBA,
                           GL_UNSIGNED_BYTE,
                           (void*) bits); 
          }
          else {
              std::cerr << "IMAGE WITH INVALID BIT DEPTH: " << bit_depth << std::endl;
              exit(-1);
          }

          // When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          // When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
          // Generate mipmaps, by the way.
          glGenerateMipmap(GL_TEXTURE_2D);

          glBindTexture(GL_TEXTURE_2D, 0);

          this->textures.insert(TexTable::value_type(tex.filename, tex));
    
          FreeImage_Unload(img);
      }
      


      root = deepcopy_assimp_tree(this, rootAiNode, scene);
   }

   Drawable::~Drawable() {}
};
