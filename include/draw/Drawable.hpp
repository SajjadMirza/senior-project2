#ifndef _DRAWABLE_H
#define _DRAWABLE_H

#include <common.hpp>

#include <unordered_map>
#include <draw/Texture.hpp>

#include <draw/Shape.hpp>

#include <MatrixStack.hpp>



#include <ModelConfig.hpp>

class Camera; // forward declaration

namespace draw {

    /* this struct is a memory leak waiting to happen */
    struct Node {
        Eigen::Matrix4f transform;
        std::vector<Shape> meshes;
        std::vector<Node*> children;
    };

    /*
      This class doesn't expose the internal hierarchy of the nodes.
      We may need to change that in the future.
    */
    class Drawable {
    public:
//        std::vector<Texture> textures_loaded;
        std::string directory;
        Node *root;
        TextureBundle texs;
        TexTable textures;
        std::string name;

        Drawable(const ModelConfig &config);
        Drawable(const aiScene *scene, std::string& dir);
        ~Drawable(); // This should probably be implemented sometime....

        const Shape *find_first_shape();

        void draw(Program *prog, MatrixStack *P, MatrixStack *MV, Camera *cam);
        void drawColor(Program *prog, MatrixStack *P, MatrixStack *MV,
                       Camera *cam);
        void drawDeferred(Program *prog, MatrixStack *M, Camera *cam);
        void drawAsLightVolume(Program *prog, MatrixStack *M, Camera *cam);
        void drawDepth(Program *prog, MatrixStack *M);
    };

    typedef std::unordered_map<uint, Drawable*> DrawableMap; 

};
#endif
