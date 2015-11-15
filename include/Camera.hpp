#ifndef _CAMERA_H
#define _CAMERA_H

#include <common.hpp>

#include <Entity.hpp>
#include <MatrixStack.hpp>
#include <Map.hpp>

static const float camera_default_collision_radius = 0.15;

class Camera
{
public:
	
    enum {
        ROTATE = 0,
        TRANSLATE,
        SCALE,
        NOTHING,
    };
	
    Camera();
    virtual ~Camera();
    void setAspect(float a) { aspect = a; };
    void setRotationFactor(float f) { rfactor = f; };
    void setTranslationFactor(float f) { tfactor = f; };
    void setScaleFactor(float f) { sfactor = f; };
    virtual void mouseReleased();
    virtual void modifierReleased();
    virtual void mouseClicked(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void mouseMoved(int x, int y);
    void applyProjectionMatrix(MatrixStack *P) const;
    void applyViewMatrix(MatrixStack *MV) const;
    Eigen::Vector2f rotations;
    Eigen::Vector3f translations;

    // DON'T CALL THIS FUNCTION!!!!!!!!!!!!!!!!!!!!!!!!!!
    void move(char c, const std::vector<Entity> &entities,
              const Map &map, int col, int row);

    float collisionRadius() const { return camera_default_collision_radius; }

    bool collides(const Entity &e);
	
protected:
    float aspect;
    float fovy;
    float znear;
    float zfar;
    float scale;
    Eigen::Vector2f mousePrev;
    int state;
    float rfactor;
    float tfactor;
    float sfactor;
};


#endif
