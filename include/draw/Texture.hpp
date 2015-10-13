#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <common.hpp>

#include <unordered_map>
#include <boost/optional.hpp>

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
    

    struct TextureBundle {
        boost::optional<Texture> diffuse;
        boost::optional<Texture> normal;
        boost::optional<Texture> displacement;
        boost::optional<Texture> occlusion;
        boost::optional<Texture> shadow;
        boost::optional<Texture> light;
        boost::optional<Texture> specular;
    };
      

    typedef std::unordered_map<std::string, Texture> TexTable;
};

#endif
