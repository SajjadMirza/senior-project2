#include <Entity.hpp>

Entity::Entity() : pos(0, 0, 0), rot(Eigen::Matrix4f::Identity()), drawable(NULL) {}
Entity::~Entity() {}

void Entity::setRotation(float angle, Eigen::Vector3f axis) {}
void Entity::setPosition(Eigen::Vector3f position) {} 

void Entity::move(Eigen::Vector3f translation) {} 
void Entity::rotate(float angle, Eigen::Vector3f axis) {}

void Entity::attachDrawable(draw::Drawable *drawable) {
   this->drawable = drawable;
}

const draw::Drawable& Entity::getDrawable() {
    return *drawable;
}

void Entity::clearDrawable() {
   this->drawable = NULL;
}
