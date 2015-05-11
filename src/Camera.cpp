#include <Camera.hpp>

Camera::Camera() :
    aspect(1.0f),
    fovy(45.0f/180.0f*M_PI),
    znear(0.1f),
    zfar(1000.0f),
    rotations(0.0, 0.0),
    translations(0.0f, 0.0f, -5.0f),
    rfactor(0.005f),
    tfactor(0.1f),
    sfactor(0.005f),
    state(Camera::NOTHING)
{
}

Camera::~Camera()
{
	
}

void Camera::mouseReleased() {
    state = Camera::NOTHING;
}

void Camera::modifierReleased() {
    if (state != Camera::NOTHING) {
        state = Camera::ROTATE;
    }
}

void Camera::mouseClicked(int x, int y, bool shift, bool ctrl, bool alt)
{
    mousePrev << x, y;
    if(shift) {
        state = Camera::TRANSLATE;
    } else if(ctrl) {
        state = Camera::SCALE;
    } else {
        state = Camera::ROTATE;
    }
}

void Camera::mouseMoved(int x, int y)
{
    Eigen::Vector2f mouseCurr(x, y);
    Eigen::Vector2f dv = mouseCurr - mousePrev;
    switch(state) {
    case Camera::ROTATE:
        rotations += rfactor * dv;
        break;
    case Camera::TRANSLATE:
        translations(0) += tfactor * dv(0);
        translations(1) -= tfactor * dv(1);
        break;
    case Camera::SCALE:
        translations(2) *= (1.0f - sfactor * dv(1));
        break;
    }
    mousePrev = mouseCurr;
}

void Camera::applyProjectionMatrix(MatrixStack *P) const
{
    P->perspective(fovy, aspect, znear, zfar);
}

void Camera::applyViewMatrix(MatrixStack *MV) const
{
    MV->rotate(rotations(1), Eigen::Vector3f(1.0f, 0.0f, 0.0f));
    MV->rotate(rotations(0), Eigen::Vector3f(0.0f, 1.0f, 0.0f));
    MV->translate(translations);
}

void Camera::move(char c) {
    Eigen::Quaternionf q1;
	Eigen::Vector3f axis1(0.0f, 1.0f, 0.0f);
	q1 = Eigen::AngleAxisf(rotations(0), axis1); 
	Eigen::Matrix4f R1 = Eigen::Matrix4f::Identity();
	R1.block<3,3>(0,0) = q1.toRotationMatrix();
    
    if (c == 's') { //w
    	Eigen::Vector3f point(0.0f, 0.0f, 1.0f);
    	point = R1.block<3,3>(0,0) * point;
    	
        translations(0) += (point(0) * tfactor);       
        translations(2) -= (point(2) * tfactor);       
    }
    if (c == 'd') { //w
    	Eigen::Vector3f point(-1.0f, 0.0f, 0.0f);
    	point = R1.block<3,3>(0,0) * point;
    	
        translations(0) += (point(0) * tfactor);     
        translations(2) -= (point(2) * tfactor);         
    }
    if (c == 'w') {
    	Eigen::Vector3f point(0.0f, 0.0f, -1.0f);
    	point = R1.block<3,3>(0,0) * point;
    	
        translations(0) += (point(0) * tfactor);       
        translations(2) -= (point(2) * tfactor); 
    }
    if (c == 'a') {
    	Eigen::Vector3f point(1.0f, 0.0f, 0.0f);
    	point = R1.block<3,3>(0,0) * point;      
    	
        translations(0) += (point(0) * tfactor);     
        translations(2) -= (point(2) * tfactor);  
    }
}
