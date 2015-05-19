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

struct ModelConfig {
    std::string model;
    std::string file;
    std::string directory;
    std::string format;
    boost::optional<float> radius_override;
    bool use_position_center_override = false;
    ModelTextureConfig textures;
    
};

#endif
