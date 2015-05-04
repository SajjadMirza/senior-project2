#ifndef _RESOURCES_H
#define _RESOURCES_H

#include <common.hpp>

#include <draw/Drawable.hpp>



namespace resource {
    
    uint import_object(const std::string& file);

    FIBITMAP* GenericLoader(const char* lpszPathName, int flag);

   
};
#endif
