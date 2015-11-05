#include <draw/Light.hpp>
#include <cmath>
#include <log.hpp>

static const float cutoff = 3.0;

float calculate_point_light_radius(const PointLight &light)
{
    float Imax = light.diffuse.maxCoeff();
    float Kl = light.linear;
    float Kq = light.quadratic;
    float Kc = light.constant;
    
    float radius = (-Kl + std::sqrt(Kl * Kl - 4 * Kq * (Kc - (256.0/cutoff) * Imax))) / (2 * Kq);
//    LOG("radius " << radius);
    return radius;
//    return 30;
}
