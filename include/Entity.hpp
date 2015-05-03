#ifndef _ENTITY_H
#define _ENTITY_H

#include <common.hpp>

#include <draw/Drawable.hpp>

class Entity {
private:
   Eigen::Vector3f pos;
   Eigen::Matrix4f rot;
   const draw::Drawable *drawable;
public:
   Entity();
   virtual ~Entity();

   void attachDrawable(draw::Drawable *drawable);
   void clearDrawable();

   void setRotation(float angle, Eigen::Vector3f axis);
   void setPosition(Eigen::Vector3f position);

   void move(Eigen::Vector3f translation);
   void rotate(float angle, Eigen::Vector3f axis);
};

#endif
