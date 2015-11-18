#ifndef _ENTITY_H
#define _ENTITY_H

#include <common.hpp>

#include <draw/Drawable.hpp>
#include <BoundingBox.hpp>

class Entity {
private:
    std::string name;
    Eigen::Vector3f pos;
    Eigen::Matrix4f rot;
    float scale;
    float radius;
    Eigen::Vector3f center;
    draw::Drawable *drawable;
    BoundingBox bounding_box;
    bool use_bounding_box;
    bool simple_draw;
public:
    bool selected;
    uint id;
    Entity();
    Entity(draw::Drawable *d);
    virtual ~Entity();
    
    void setName(std::string str);
    std::string getName();

    void attachDrawable(draw::Drawable *drawable);
    void clearDrawable();

    draw::Drawable& getDrawable();

    void setRotation(float angle, Eigen::Vector3f axis);
    void setRotationMatrix(Eigen::Matrix4f mat);
    void setPosition(Eigen::Vector3f position);

    inline Eigen::Vector3f getPosition() const { return pos; }

    void move(Eigen::Vector3f translation);
    void rotate(float angle, Eigen::Vector3f axis);

    void calculate_center_and_radius();
    inline void setCenter(Eigen::Vector3f c) { center = c; }
    inline void setRadius(float r) { radius = r; }
    float getRadius() const;
    Eigen::Vector3f getCenterWorld() const;

    const Eigen::Matrix4f& getRotation() const;

    const BoundingBox &getBoundingBox() const;
    void generateBoundingBox();
    bool useBoundingBox() const;
    void setUseBoundingBox(bool use);
    void setSimpleDraw(bool simple);

    inline float getScale() const { return scale; }
    inline void setScale(float s) { scale = s; }
};

#endif
