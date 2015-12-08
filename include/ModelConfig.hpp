#ifndef _MODELCONFIG_HPP
#define _MODELCONFIG_HPP

#include <common.hpp>

#include <boost/optional.hpp>

struct ModelTextureConfig {
    std::string diffuse;
    std::string normal;
    std::string displacement;
    std::string occlusion;
    std::string shadow;
    std::string light;
    std::string specular;
};

struct TransformConfig {
    // rotations in degrees
    float xrot;
    float yrot;
    float zrot;

    float xpos;
    float ypos;
    float zpos;

    float scale;

    TransformConfig() {
        xrot = 0;
        yrot = 0;
        zrot = 0;

        xpos = 0;
        ypos = 0;
        zpos = 0;

        scale = 0;
    }
};


struct ModelConfig {
    std::string model;
    std::string file;
    std::string directory;
    std::string format;
    std::string description;
    boost::optional<float> radius_override;
    bool use_position_center_override;
    ModelTextureConfig textures;
    TransformConfig transforms;
    
    ModelConfig() {
        use_position_center_override = false;
    }
};

#endif
