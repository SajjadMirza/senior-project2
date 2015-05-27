#ifndef _OVCAMERA_H
#define _OVCAMERA_H

#include <Camera.hpp>

class OverviewCamera : public Camera
{
public:
	OverviewCamera();
	virtual ~OverviewCamera();
	void mouseReleased();
    void modifierReleased();
    void mouseClicked(int x, int y, bool shift, bool ctrl, bool alt);
    void mouseMoved(int x, int y);
};
#endif