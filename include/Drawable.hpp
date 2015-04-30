#ifndef _DRAWABLE_H
#define _DRAWABLE_H

#include <common.hpp>

class Drawable {
private:
   struct Node {
      std::vector<Node*> children;
      std::vector<Shape*> meshes;
   };

   Node root;
public:
   Drawable(const aiScene *scene);
   ~Drawable();
};


#endif
