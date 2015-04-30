#ifndef _SHAPE_H
#define _SHAPE_H

#include <common.hpp>

namespace draw {

   class Shape {
   private:
      GLuint ver_buf;
      GLuint nor_buf;
      GLuint tex_buf;
      GLuint ind_buf;

   public:
      Shape();
      ~Shape();
      void init(const aiMesh& mesh);
   };

}

#endif
