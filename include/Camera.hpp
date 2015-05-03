#ifndef _CAMERA_H
#define _CAMERA_H

#include <common.hpp>

#include <MatrixStack.hpp>

class Camera
{
public:
	
	enum {
		ROTATE = 0,
		TRANSLATE,
		SCALE
	};
	
	Camera();
	virtual ~Camera();
	void setAspect(float a) { aspect = a; };
	void setRotationFactor(float f) { rfactor = f; };
	void setTranslationFactor(float f) { tfactor = f; };
	void setScaleFactor(float f) { sfactor = f; };
	void mouseClicked(int x, int y, bool shift, bool ctrl, bool alt);
	void mouseMoved(int x, int y);
	void applyProjectionMatrix(MatrixStack *P) const;
	void applyViewMatrix(MatrixStack *MV) const;
	Eigen::Vector2f rotations;
	Eigen::Vector3f translations;
	
private:
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
