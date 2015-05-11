#include <common.hpp>

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

    ModelTextureConfig textures;
    
};
