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
};

struct TransformConfig {
    // rotations in degrees
    float xrot = 0;
    float yrot = 0;
    float zrot = 0;

    float xpos = 0;
    float ypos = 0;
    float zpos = 0;
};


struct ModelConfig {
    std::string model;
    std::string file;
    std::string directory;
    std::string format;
    boost::optional<float> radius_override;
    bool use_position_center_override = false;
    ModelTextureConfig textures;
    TransformConfig transforms;
};

#endif
