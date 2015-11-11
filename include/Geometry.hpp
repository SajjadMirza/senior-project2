#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

#include <Eigen/Dense>

struct Plane {
    float a, b, c, d;
};

struct Frustum {
    Plane left;
    Plane right;
    Plane top;
    Plane bottom;
    Plane near;
    Plane far;
};

enum Containment {
    INSIDE,
    OUTSIDE,
    INTERSECT,
};


void normalize_plane(Plane *plane);
void normalize(Frustum *frustum);
void extract_planes(Frustum *frustum, const Eigen::Matrix4f &matrix);
float classify_point(const Plane &plane, const Eigen::Vector3f &point);
Containment check_frustum_sphere(const Frustum &frustum, const Eigen::Vector3f &center, float radius);
Containment check_frustum_point(const Frustum &frustum, const Eigen::Vector3f &center);

#endif
