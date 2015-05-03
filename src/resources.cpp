// function imports object using assimp

#include <resources.hpp>

extern draw::DrawableMap drawable_map;

namespace resource {
    
    static uint drawable_counter = 0;
    
    // returns zero on fail, positive handle for drawable on success
    uint import_object(const std::string& file) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(file,
                                                 aiProcess_CalcTangentSpace       | 
                                                 aiProcess_Triangulate            |
                                                 aiProcess_JoinIdenticalVertices  |
                                                 aiProcess_SortByPType);
   
        if (!scene) {
            std::cerr << importer.GetErrorString() << std::endl;
            return 0;
        }

        draw::Drawable *drawable = new draw::Drawable(scene);
        drawable_map.insert(draw::DrawableMap::value_type(++drawable_counter, drawable));
        
   
        return drawable_counter;
    }

}
