#include <iostream>
#include <cassert>
#include <Program.hpp>
#include <GLSL.hpp>

using namespace std;

Program::Program() :
	pid(0),
	vShaderName(""),
	fShaderName(""),
        gShaderName(""),
        use_gs(false)
{
	
}

Program::~Program()
{
	
}

void Program::setShaderNames(const string &v, const string &f)
{
	vShaderName = v;
	fShaderName = f;
}

void Program::setShaderNames(const string &v, const string &f, const string &g)
{
    vShaderName = v;
    fShaderName = f;
    gShaderName = g;
    use_gs = true;
}

bool Program::init()
{
	GLint rc;
	
	// Create shader handles
	GLuint VS = glCreateShader(GL_VERTEX_SHADER);
	GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);
        GLuint GS = 0;
        if (use_gs) {
            GS = glCreateShader(GL_GEOMETRY_SHADER);
        }
	
	// Read shader sources
	const char *vshader = GLSL::textFileRead(vShaderName.c_str());
	const char *fshader = GLSL::textFileRead(fShaderName.c_str());
	glShaderSource(VS, 1, &vshader, NULL);
	glShaderSource(FS, 1, &fshader, NULL);

        const char *gshader = NULL; 
        if (use_gs) {
            gshader = GLSL::textFileRead(gShaderName.c_str());
            glShaderSource(GS, 1, &gshader, NULL);
        }
	
	// Compile vertex shader
	glCompileShader(VS);
	glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
	GLSL::printShaderInfoLog(VS);
	if(!rc) {
		printf("Error compiling vertex shader %s\n", vShaderName.c_str());
		return false;
	}
	
	// Compile fragment shader
	glCompileShader(FS);
	glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
	GLSL::printShaderInfoLog(FS);
	if(!rc) {
		printf("Error compiling fragment shader %s\n", fShaderName.c_str());
		return false;
	}

        // Compile geometry shader
        if (use_gs) {
            glCompileShader(GS);
            glGetShaderiv(GS, GL_COMPILE_STATUS, &rc);
            GLSL::printShaderInfoLog(GS);
            if(!rc) {
		printf("Error compiling geometry shader %s\n", gShaderName.c_str());
		return false;
            }
        }
	
	// Create the program and link
	pid = glCreateProgram();
	glAttachShader(pid, VS);
	glAttachShader(pid, FS);
        if (use_gs) {
            glAttachShader(pid, GS);
        }
	glLinkProgram(pid);
	glGetProgramiv(pid, GL_LINK_STATUS, &rc);
	GLSL::printProgramInfoLog(pid);
	if(!rc && !use_gs) {
            printf("Error linking shaders %s and %s\n", vShaderName.c_str(), fShaderName.c_str());
		return false;
	}
        if (!rc && use_gs) {
            printf("Error linking shaders %s and %s and %s\n", 
                   vShaderName.c_str(), 
                   fShaderName.c_str(),
                   gShaderName.c_str());
            return false;
        }
	
	GLSL::printError();
	assert(glGetError() == GL_NO_ERROR);

        
	
	return true;
}

void Program::bind()
{
	glUseProgram(pid);
}

void Program::unbind()
{
	glUseProgram(0);
}

void Program::addAttribute(const string &name)
{
    attributes[name] = GLSL::getAttribLocation(pid, name.c_str());
    LOG("Added attribute: " << name << ": " << attributes[name]);
}

void Program::addUniform(const string &name)
{
    uniforms[name] = GLSL::getUniformLocation(pid, name.c_str());
    LOG("Added uniform: " << name << ": " << uniforms[name]);
}

GLint Program::getAttribute(const string &name) const
{
	map<string,GLint>::const_iterator attribute = attributes.find(name.c_str());
	if(attribute == attributes.end()) {
		cout << name << " is not an attribute variable" << endl;
		return 0;
	}
	return attribute->second;
}

GLint Program::getUniform(const string &name) const
{
	map<string,GLint>::const_iterator uniform = uniforms.find(name.c_str());
	if(uniform == uniforms.end()) {
		cout << name << " is not a uniform variable" << endl;
		return 0;
	}
	return uniform->second;
}
