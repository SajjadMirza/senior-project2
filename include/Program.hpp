#pragma  once
#ifndef __Program__
#define __Program__

#include <common.hpp>
#include <map>
#include <string>
#include <stdio.h>

class Program
{
public:
    Program();
    virtual ~Program();
	
    void setShaderNames(const std::string &v, const std::string &f);
    void setShaderNames(const std::string &v, const std::string &f, const std::string &g);
    virtual bool init();
    virtual void bind();
    virtual void unbind();

    void addAttribute(const std::string &name);
    void addUniform(const std::string &name);
    GLint getAttribute(const std::string &name) const;
    GLint getUniform(const std::string &name) const;
	
protected:
    std::string vShaderName;
    std::string fShaderName;
    std::string gShaderName;
private:
    GLuint pid;
    std::map<std::string,GLint> attributes;
    std::map<std::string,GLint> uniforms;
    bool use_gs;
};

#endif
