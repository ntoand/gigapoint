#ifndef SHADER_H
#define SHADER_H

#include "Utils.h"

#include <omegaGl.h>

#include <list>
#include <string>

using namespace std;

class Shader
{
public:
    Shader(string name);
    ~Shader();

    Shader& load(string shader, list<string> attributes, list<string> uniforms, const Option* option);
    Shader& unload();
    Shader& bind();
    Shader& unbind();
    Shader& setup();
    Shader& setupLocations(list<string> _attributes, list<string> _uniforms);

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
    
    map<string, unsigned int> attributes;
    map<string, unsigned int> uniforms;
};

#endif // SHADER_H
