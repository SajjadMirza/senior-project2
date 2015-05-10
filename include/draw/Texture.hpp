#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <common.hpp>

#include <unordered_map>

namespace draw {
    enum TexType {
        NONE,
        DIFFUSE,
        SPECULAR,
        NORMAL,
        BUMP,
        LIGHT,
        SHADOW,
        AMBIENT,
    };
    
    class Texture {
    public:
        std::string filename;
        TexType type;
        GLuint tid;

        Texture() : tid(0), type(TexType::NONE) {}
        ~Texture() {}
    };

    struct TexturePackage {
        std::string diffuse;
        std::string specular;
        std::string normal;
        std::string displacement;
        std::string occlusion;
    };

    typedef std::unordered_map<std::string, Texture> TexTable;
};

#endif
