// function imports object using assimp

#include <resources.hpp>

#include <boost/filesystem.hpp>


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
        
        boost::filesystem::path p(file);

        std::cout << "has parent path: " << p.has_parent_path() << std::endl; 
        
        boost::filesystem::path dir = p.parent_path();
        
        std::cout << "loaded object from directory: " << dir.string() << std::endl;

        std::string str = dir.string();

        draw::Drawable *drawable = new draw::Drawable(scene, str);

        drawable_map.insert(draw::DrawableMap::value_type(++drawable_counter, drawable));
        
   
        return drawable_counter;
    }


    /*
     */
    FIBITMAP* GenericLoader(const char* lpszPathName, int flag) {
        FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
        // check the file signature and deduce its format
        // (the second argument is currently not used by FreeImage)
        fif = FreeImage_GetFileType(lpszPathName, 0);
        if(fif == FIF_UNKNOWN) {
            // no signature ?
            // try to guess the file format from the file extension
            fif = FreeImage_GetFIFFromFilename(lpszPathName);
        }
        // check that the plugin has reading capabilities ...
        if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
            // ok, let's load the file
            FIBITMAP *dib = FreeImage_Load(fif, lpszPathName, flag);
            // unless a bad file format, we are done !
            return dib;
        }
        return NULL;
    }


}
