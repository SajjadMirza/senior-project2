#ifndef _DRAWABLE_H
#define _DRAWABLE_H

#include <common.hpp>

#include <draw/Shape.hpp>

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
      Node *root;

      Drawable(const aiScene *scene);
      ~Drawable(); // This should probably be implemented sometime....

   };

};
#endif
