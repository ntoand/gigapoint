#include "Shader.h"
#include "Utils.h"

Shader::Shader(string name)
{
    this->name = name;
    this->uid = -1;
    this->vertex = NULL;
    this->fragment = NULL;
}

Shader::~Shader()
{
    attributes.clear();
    uniforms.clear();
    delete [] vertex;
    delete [] geom;
    delete [] fragment;
}

Shader& Shader::load(string shaderPrefix, list<string> attributes, list<string> uniforms)
{;
    vertex = Utils::getFileContent(shaderPrefix+".vert");
#ifdef USE_GEOM
    geom = Utils::getFileContent(shaderPrefix+".geom");
#endif
    fragment = Utils::getFileContent(shaderPrefix+".frag");
    printf("Compile shader: %s\n", shaderPrefix.c_str());
#ifdef PRINT_DEBUG
    printf("vertex shader:\n%s\n", vertex);
    //printf("geom shader:\n%s\n", geom);
    printf("fragment shader:\n%s\n", fragment);
#endif
    setup();
    setupLocations(attributes, uniforms);

    return *this;
}

Shader& Shader::setup()
{
#ifdef USE_GEOM
    if (vertex == NULL || geom == NULL || fragment == NULL)
#else
    if (vertex == NULL || fragment == NULL)
#endif
    {
        printf("Error: Unable to load shader\n");
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

 #ifdef USE_GEOM
    unsigned int gshader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(gshader, 1, &geom, NULL);
    glCompileShader(gshader);
    glGetShaderiv(gshader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE)
    {
        glGetShaderiv(gshader, GL_INFO_LOG_LENGTH, &logSize);
        log = new char[logSize - 1];
        glGetShaderInfoLog(gshader, logSize, &logSize, log);
        printf("Error: Unable to compile geometry shader\n %s", log);
        delete log;
        exit(-1);
    }
    glAttachShader(pProgram, gshader);
 #endif

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

Shader& Shader::setupLocations(list<string> _attributes, list<string> _uniforms)
{
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

Shader& Shader::bind()
{
    glUseProgram(uid);

    return *this;
}

string& Shader::getName()
{
    return name;
}

unsigned int Shader::attribute(string name)
{
    return attributes.at(name);
}

unsigned int Shader::uniform(string name)
{
    return uniforms.at(name);
}

bool Shader::hasAttribute(string name)
{
    if (attributes.find(name) == attributes.end())
        return false;
    return true;
}

bool Shader::hasUniform(string name)
{
    if (uniforms.find(name) == uniforms.end())
        return false;
    return true;
}

void Shader::transmitUniform(string name, int i)
{
    glUniform1i(uniforms.at(name), i);
}

void Shader::transmitUniform(string name, float f)
{
    glUniform1f(uniforms.at(name), f);
}

void Shader::transmitUniform(string name, float f1, float f2)
{
    glUniform2f(uniforms.at(name), f1, f2);
}

void Shader::transmitUniform(string name, float f1, float f2, float f3)
{
    glUniform3f(uniforms.at(name), f1, f2, f3);
}

void Shader::transmitUniform(string name, const float mat[16])
{
    glUniformMatrix4fv(uniforms.at(name), 1, GL_FALSE, mat);
}
