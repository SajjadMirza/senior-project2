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

    int load_config(ModelConfig *config, const std::string &path) {
        std::vector<YAML::Node> documents = YAML::LoadAllFromFile(path);

        if (documents.empty()) {
            return -1;
        }
        
        const YAML::Node &doc = documents[0];
        ModelConfig conf;
        conf.model = doc["model"].as<std::string>();
        conf.file = doc["file"].as<std::string>();
        conf.directory = doc["directory"].as<std::string>();
        conf.format = doc["format"].as<std::string>();

        if (doc["overrides"]) {
            YAML::Node overrides = doc["overrides"];
            if (overrides["radius"]) {
                conf.radius_override = overrides["radius"].as<float>();
            }
            if (overrides["radius"]) {
                conf.use_position_center_override = true;
            }
        }

        if (doc["transforms"]) {
                
            if (doc["transforms"]["rotations"]) {
                YAML::Node rots = doc["transforms"]["rotations"];
                float xdeg = rots["xaxis"] ? rots["xaxis"].as<float>() : 0;
                float ydeg = rots["yaxis"] ? rots["yaxis"].as<float>() : 0;
                float zdeg = rots["zaxis"] ? rots["zaxis"].as<float>() : 0;

                conf.transforms.xrot = xdeg;
                conf.transforms.yrot = ydeg;
                conf.transforms.zrot = zdeg;
            }
                
            if (doc["transforms"]["translations"]) {
                YAML::Node trans = doc["transforms"]["translations"];
                float xpos = trans["xaxis"] ? trans["xaxis"].as<float>() : 0;
                float ypos = trans["yaxis"] ? trans["yaxis"].as<float>() : 0;
                float zpos = trans["zaxis"] ? trans["zaxis"].as<float>() : 0;

                conf.transforms.xpos = xpos;
                conf.transforms.ypos = ypos;
                conf.transforms.zpos = zpos;
            }

            if (doc["transforms"]["scale"]) {
                conf.transforms.scale = doc["transforms"]["scale"].as<float>();
            }
                
        }
            

        if (doc["textures"]) {
            YAML::Node tex = doc["textures"];
            std::string key;
            key = "diffuse";
            if (tex[key]) {
                conf.textures.diffuse = tex[key].as<std::string>();
            }

            key = "normal";
            if (tex[key]) {
                conf.textures.normal = tex[key].as<std::string>();
            }
                
            key = "displacement";
            if (tex[key]) {
                conf.textures.displacement = tex[key].as<std::string>();
            }

            key = "occlusion";
            if (tex[key]) {
                conf.textures.occlusion = tex[key].as<std::string>();
            }

            key = "shadow";
            if (tex[key]) {
                conf.textures.shadow = tex[key].as<std::string>();
            }

            key = "light";
            if (tex[key]) {
                conf.textures.light = tex[key].as<std::string>();
            }
        }

        *config = conf;
        return 0;
    }
    

    /*
      Returns the number of configuration objects loaded.
      Return value is negative if an error occured while loading the objects,
      zero if no objects were found at all.
      
    */
    int load_model_configs(std::vector<ModelConfig> *configs, const std::string &path) {
        int ret = 0;

        std::vector<YAML::Node> documents = YAML::LoadAllFromFile(path);

        if (documents.empty()) {
            return 0;
        }

        configs->reserve(documents.size());
        
        for (auto it = documents.begin(); it != documents.end(); it++) {
            const YAML::Node &doc = *it;
            ModelConfig conf;
            conf.model = doc["model"].as<std::string>();
            conf.file = doc["file"].as<std::string>();
            conf.directory = doc["directory"].as<std::string>();
            conf.format = doc["format"].as<std::string>();

            if (doc["overrides"]) {
                YAML::Node overrides = doc["overrides"];
                if (overrides["radius"]) {
                    conf.radius_override = overrides["radius"].as<float>();
                }
                if (overrides["radius"]) {
                    conf.use_position_center_override = true;
                }
            }

            if (doc["transforms"]) {
                
                if (doc["transforms"]["rotations"]) {
                    YAML::Node rots = doc["transforms"]["rotations"];
                    float xdeg = rots["xaxis"] ? rots["xaxis"].as<float>() : 0;
                    float ydeg = rots["yaxis"] ? rots["yaxis"].as<float>() : 0;
                    float zdeg = rots["zaxis"] ? rots["zaxis"].as<float>() : 0;

                    conf.transforms.xrot = xdeg;
                    conf.transforms.yrot = ydeg;
                    conf.transforms.zrot = zdeg;
                }
                
                if (doc["transforms"]["translations"]) {
                    YAML::Node trans = doc["transforms"]["translations"];
                    float xpos = trans["xaxis"] ? trans["xaxis"].as<float>() : 0;
                    float ypos = trans["yaxis"] ? trans["yaxis"].as<float>() : 0;
                    float zpos = trans["zaxis"] ? trans["zaxis"].as<float>() : 0;

                    conf.transforms.xpos = xpos;
                    conf.transforms.ypos = ypos;
                    conf.transforms.zpos = zpos;
                }

                if (doc["transforms"]["scale"]) {
                    conf.transforms.scale = doc["transforms"]["scale"].as<float>();
                }
                
            }
            

            if (doc["textures"]) {
                YAML::Node tex = doc["textures"];
                std::string key;
                key = "diffuse";
                if (tex[key]) {
                    conf.textures.diffuse = tex[key].as<std::string>();
                }

                key = "normal";
                if (tex[key]) {
                    conf.textures.normal = tex[key].as<std::string>();
                }
                
                key = "displacement";
                if (tex[key]) {
                    conf.textures.displacement = tex[key].as<std::string>();
                }

                key = "occlusion";
                if (tex[key]) {
                    conf.textures.occlusion = tex[key].as<std::string>();
                }

                key = "shadow";
                if (tex[key]) {
                    conf.textures.shadow = tex[key].as<std::string>();
                }

                key = "light";
                if (tex[key]) {
                    conf.textures.light = tex[key].as<std::string>();
                }
            }

            configs->push_back(conf);
            ++ret;
        }

        return ret;
    }

    void tex_image(GLint internalFormat, GLsizei width, GLsizei height,
                   GLenum format, const void *data) {
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     internalFormat,
                     width,
                     height,
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     data);
    }
    
    void rgb_tex_image(GLsizei width, GLsizei height, const void *data) {
        tex_image(GL_RGB, width, height, GL_BGR, data);
    }

    void rgba_tex_image(GLsizei width, GLsizei height, const void *data) {
        tex_image(GL_RGBA, width, height, GL_BGRA, data);
    }

    void load_texture_from_file(const std::string &path, GLuint *tid_ptr) {
        FIBITMAP *img = resource::GenericLoader(path.c_str(), 0);
        uint bit_depth = FreeImage_GetBPP(img);
        BYTE *img_data = FreeImage_GetBits(img);

        glGenTextures(1, tid_ptr);
        glBindTexture(GL_TEXTURE_2D, *tid_ptr);

        int width = FreeImage_GetWidth(img);
        int height = FreeImage_GetHeight(img);

        if (bit_depth == RGB_BITS) {
            rgb_tex_image(width, height, img_data);
        }
        else if (bit_depth == RGBA_BITS) {
            rgba_tex_image(width, height, img_data);
        }
        else {
            std::cerr << "WARNING: " << path << " HAS INVALID BIT DEPTH: "
                      << bit_depth << std::endl;
            rgb_tex_image(width, height, img_data);
        }

        // When MAGnifying the image (no bigger mipmap available),
        // use LINEAR filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // When MINifying the image, use a LINEAR blend of two mipmaps,
        // each filtered LINEARLY too
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        // Generate mipmaps, by the way.
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        FreeImage_Unload(img);

        std::cout << "done loading texture" << std::endl;
    }


}
