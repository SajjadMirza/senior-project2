#ifndef _ENTITY_H
#define _ENTITY_H

#include <common.hpp>

#include <draw/Drawable.hpp>

class Entity {
private:
    Eigen::Vector3f pos;
    Eigen::Matrix4f rot;
    float radius;
    Eigen::Vector3f center;
    draw::Drawable *drawable;
public:
    Entity();
    Entity(draw::Drawable *d);
    virtual ~Entity();
    
    void attachDrawable(draw::Drawable *drawable);
    void clearDrawable();

    draw::Drawable& getDrawable();

    void setRotation(float angle, Eigen::Vector3f axis);
    void setPosition(Eigen::Vector3f position);

    Eigen::Vector3f getPosition() { return pos; }

    void move(Eigen::Vector3f translation);
    void rotate(float angle, Eigen::Vector3f axis);

    void calculate_center_and_radius();
    float getRadius();
    Eigen::Vector3f getCenterWorld();
    bool collides(const Camera &cam);
};

#endif
