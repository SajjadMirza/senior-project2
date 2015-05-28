#ifndef _BASIC_GEOMETRY_HPP
#define _BASIC_GEOMETRY_HPP

struct Sphere {
    Eigen::Vector3f center;
    float radius;
};

struct Box {
    Eigen::Vector3f min, max;
    inline Eigen::Vector3f center() const { return (max + min) / 2.0f; }
};

#endif
