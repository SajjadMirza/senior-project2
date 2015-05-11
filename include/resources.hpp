#ifndef _RESOURCES_H
#define _RESOURCES_H

#include <common.hpp>
#include <yaml-cpp/yaml.h>

#include <draw/Drawable.hpp>
#include <ModelConfig.hpp>




namespace resource {
    
    uint import_object(const std::string& file);

    FIBITMAP* GenericLoader(const char* lpszPathName, int flag);

    int load_model_configs(std::vector<ModelConfig> *configs, std::string path);
};
#endif
