#ifndef _GAME_MATH_HPP
#define _GAME_MATH_HPP

#include <Eigen/Dense>
#include <cmath>

namespace math {
    inline float deg_to_rad(float deg)
    {
        float rad = (M_PI / 180.0f) * deg;
        return rad;
    }


    inline Eigen::Matrix4f perspective(float fov, float aspect, float near, float far)
    {
        float radf = deg_to_rad(fov);
        float tanHalfFov = std::tan(radf / 2.0f);
        Eigen::Matrix4f result = Eigen::Matrix4f::Zero();
        result(0,0) = 1.0 / (aspect * tanHalfFov);
        result(1,1) = 1.0 / tanHalfFov;
        result(2,2) = -(far + near) / (far - near);
        result(3,2) = -1.0;
        result(2,3) = -(2.0 * far * near) / (far - near);
        return result;
    }

    inline Eigen::Matrix4f lookAt(const Eigen::Vector3f &eye, 
                                  const Eigen::Vector3f &center,
                                  const Eigen::Vector3f &up)
    {
        Eigen::Vector3f f = (center - eye).normalized();
        Eigen::Vector3f u = up.normalized();
        Eigen::Vector3f s = f.cross(u).normalized();
        u = s.cross(f);

        Eigen::Matrix4f result;
        result <<
            s.x(),  s.y(),  s.z(), -s.dot(eye),
            u.x(),  u.y(),  u.z(), -u.dot(eye),
           -f.x(), -f.y(), -f.z(),  f.dot(eye),
                0,      0,      0,           1;
        
        return result;
    }

    inline float lerp(float a, float b, float f)
    {
        return a + f * (b - a);
    }
};

#endif
