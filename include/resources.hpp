#ifndef _RESOURCES_H
#define _RESOURCES_H

#include <common.hpp>
#include <yaml-cpp/yaml.h>

#include <draw/Drawable.hpp>
#include <ModelConfig.hpp>


namespace resource {

    const std::string menu_music_path = "resources/sounds/Ode_to_Joy.ogg";
    
    uint import_object(const std::string& file);

    FIBITMAP* GenericLoader(const char* lpszPathName, int flag);

    int load_model_configs(std::vector<ModelConfig> *configs, const std::string &path);

    void load_texture_from_file(const std::string &path, GLuint *tid_ptr);
};


#endif
