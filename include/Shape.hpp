#ifndef _MESH_H
#define _MESH_H

#include <GLFW/glfw3.h>
#include <assimp/scene.h>

#include <assert.h>

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

#endif
