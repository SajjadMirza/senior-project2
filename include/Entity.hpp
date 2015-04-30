#ifndef _ENTITY_H
#define _ENTITY_H

#include <common.hpp>

#include <Drawable.hpp>

class Entity {
private:
   Eigen::Vector3f pos;
   Eigen::Matrix4f rot;
   Drawable *drawable;
public:
   Entity();
   virtual ~Entity();
};

#endif
