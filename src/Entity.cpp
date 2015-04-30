#include <Entity.hpp>

Entity::Entity() : pos(0, 0, 0), rot(Eigen::Matrix4f::Identity()), drawable(NULL) {}
Entity::~Entity() {}

void setRotation(float angle, Eigen::Vector3f axis) {}
void setPosition(Eigen::Vector3f position) {} 

void move(Eigen::Vector3f translation) {} 
void rotate(float angle, Eigen::Vector3f axis);
