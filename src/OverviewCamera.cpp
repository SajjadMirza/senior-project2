#include <OverviewCamera.hpp>

OverviewCamera::OverviewCamera() : Camera()
{
    aspect       = 1.0f;
    fovy         = 45.0f/180.0f*M_PI;
    znear        = 0.1f;
    zfar         = 1000.0f;
    rotations    = Eigen::Vector2f(0.0, M_PI/2.0f);
    translations = Eigen::Vector3f(-4.0f, -30.0f, -27.0f);
    rfactor      = 0.005f;
    tfactor      = 0.5f;
    sfactor      = 0.005f;
    state        = Camera::NOTHING;
}

OverviewCamera::~OverviewCamera()
{
	
}

void OverviewCamera::mouseReleased() {}
void OverviewCamera::modifierReleased() {}
void OverviewCamera::mouseClicked(int x, int y, bool shift, bool ctrl, bool alt) {}
void OverviewCamera::mouseMoved(int x, int y) {}