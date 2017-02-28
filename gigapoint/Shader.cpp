#include "Shader.h"
#include "Utils.h"

#include <iostream>

using namespace std;

namespace gigapoint {

Shader::Shader(string name) {
    this->name = name;
    this->uid = -1;
    this->vertex = NULL;
    this->fragment = NULL;
}

Shader::~Shader() {
    attributes.clear();
    uniforms.clear();
    delete [] vertex;
    delete [] fragment;
}

Shader& Shader::load(string shaderPrefix, list<string> attributes, list<string> uniforms, const Option* option) {

    unload();

    string ver = "#version 120\n";
    if(option->sizeType == SIZE_FIXED)
        ver.append("#define FIXED_POINT_SIZE\n");
    
    if(option->material == MATERIAL_RGB)
        ver.append("#define MATERIAL_RGB\n");
    else if (option->material == MATERIAL_ELEVATION)
        ver.append("#define MATERIAL_ELEVATION\n");

    ver.append(Utils::getFileContent(shaderPrefix+".vert"));
    vertex = ver.c_str();
    //cout << "vertex shader: " << endl << vertex << endl;

    string fra = "#version 120\n";
    if(option->quality == QUALITY_SQUARE)
        fra.append("#define SQUARE_POINT_SHAPE\n");
    fra.append(fragment = Utils::getFileContent(shaderPrefix+".frag"));
    fragment = fra.c_str();
    //cout << "fragment shader: " << fragment << endl;

    cout << "compile shader: " << shaderPrefix << endl;

#ifdef PRINT_DEBUG
    printf("vertex shader:\n%s\n", vertex);
    printf("fragment shader:\n%s\n", fragment);
#endif

    setup();
    setupLocations(attributes, uniforms);

    return *this;
}

Shader& Shader::unload() {
    glUseProgram(0);
    if(uid != -1)
        glDeleteProgram(uid);           
    uid = -1;
    attributes.clear();
    uniforms.clear();
    return *this;
}

Shader& Shader::setup() {
    if (vertex == NULL || fragment == NULL)
    {
        printf("Error: Unable to load shader");
        exit(-1);
    }

    int status, logSize;
    char* log;
    unsigned int pProgram;

    pProgram = glCreateProgram();

    unsigned int vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vertex, NULL);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE)
    {
        glGetShaderiv(vshader, GL_INFO_LOG_LENGTH, &logSize);
        log = new char[logSize - 1];
        glGetShaderInfoLog(vshader, logSize, &logSize, log);
        printf("Error: Unable to compile vertex shader\n %s", log);
        delete log;
        exit(-1);
    }
    glAttachShader(pProgram, vshader);

    unsigned int fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fragment, NULL);
    glCompileShader(fshader);
    if(status != GL_TRUE)
    {
        glGetShaderiv(fshader, GL_INFO_LOG_LENGTH, &logSize);
        log = new char[logSize - 1];
        glGetShaderInfoLog(fshader, logSize, &logSize, log);
        printf("Error: Unable to compile fragment shader\n  %s", log);
        delete log;
        exit(-1);
    }
    glAttachShader(pProgram, fshader);

    glLinkProgram(pProgram);
    glGetProgramiv(pProgram, GL_LINK_STATUS, &status);
    if(status != GL_TRUE)
    {
        glGetProgramiv(pProgram, GL_INFO_LOG_LENGTH, &logSize);
        log = new char[logSize - 1];
        glGetProgramInfoLog(pProgram, logSize, &logSize, log);
        printf("Error: Unable to link program shader \n %s", log);
        exit(-1);
    }

    uid = pProgram;

    return *this;
}

Shader& Shader::setupLocations(list<string> _attributes, list<string> _uniforms) { 
    bind();

    for (list<string>::iterator it=_attributes.begin(); it != _attributes.end(); ++it)
    {
        string attribute = *it;
        attributes.insert(pair<string, unsigned int> (attribute, glGetAttribLocation(uid, attribute.c_str())));
    }

    for (list<string>::iterator it=_uniforms.begin(); it != _uniforms.end(); ++it)
    {
        string uniform = *it;
        uniforms.insert(pair<string, unsigned int> (uniform, glGetUniformLocation(uid, uniform.c_str())));
    }

    return *this;
}

Shader& Shader::bind() {
    glUseProgram(uid);
    return *this;
}

Shader& Shader::unbind() {
    glUseProgram(0);
    return *this;
}

string& Shader::getName() {
    return name;
}

unsigned int Shader::attribute(string name) {
    return attributes.at(name);
}

unsigned int Shader::uniform(string name) {
    return uniforms.at(name);
}

bool Shader::hasAttribute(string name) {
    if (attributes.find(name) == attributes.end())
        return false;
    return true;
}

bool Shader::hasUniform(string name) {
    if (uniforms.find(name) == uniforms.end())
        return false;
    return true;
}

void Shader::transmitUniform(string name, int i) {
    glUniform1i(uniforms.at(name), i);
}

void Shader::transmitUniform(string name, float f) {
    glUniform1f(uniforms.at(name), f);
}

void Shader::transmitUniform(string name, float f1, float f2) {
    glUniform2f(uniforms.at(name), f1, f2);
}

void Shader::transmitUniform(string name, float f1, float f2, float f3) {
    glUniform3f(uniforms.at(name), f1, f2, f3);
}

void Shader::transmitUniform(string name, const float mat[16]) {
    glUniformMatrix4fv(uniforms.at(name), 1, GL_FALSE, mat);
}

}; //namespace gigapoint
