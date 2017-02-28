#ifndef SHADER_H
#define SHADER_H

#include "Utils.h"

#include <omegaGl.h>

#include <list>
#include <string>

using namespace std;

namespace gigapoint {

class Shader
{
public:
    Shader(string name);
    ~Shader();

    Shader& load(string shader, std::list<string> attributes, std::list<string> uniforms, const Option* option);
    Shader& unload();
    Shader& bind();
    Shader& unbind();
    Shader& setup();
    Shader& setupLocations(std::list<string> _attributes, std::list<string> _uniforms);

    string& getName();

    unsigned int attribute(string name);
    unsigned int uniform(string name);

    bool hasAttribute(string name);
    bool hasUniform(string name);

    void transmitUniform(string name, int i);
    void transmitUniform(string name, float f);
    void transmitUniform(string name, float f1, float f2);
    void transmitUniform(string name, float f1, float f2, float f3);
    void transmitUniform(string name, const float mat[16]);

private:
    string name;
    unsigned int uid;
    const char* vertex;
    const char* fragment;
    
    std::map<string, unsigned int> attributes;
    std::map<string, unsigned int> uniforms;
};

}; //namespace gigapoint

#endif // SHADER_H
