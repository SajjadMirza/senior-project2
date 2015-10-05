#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <types.h>
#include <Eigen/Dense>

struct BaseLight {
   
protected:
    BaseLight();
};

struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;

    // Attenuation variables http://learnopengl.com/#!Lighting/Light-casters
    float constant;
    float linear;
    float quadratic;
    vec3 position;
};

struct DirectionalLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;

    vec3 direction;
};

struct SpotLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;

    vec3 position;
    vec3 direction;
    float innerCutoff;
    float outerCutoff;
};

#endif
