#ifndef TEXTURE_H
#define TEXTURE_H

#include "Utils.h"

#include <string>

#include <omegaGl.h>

using namespace std;

namespace gigapoint {

class ColorTexture
{
public:
    ColorTexture(const char* filename, unsigned int _format=GL_RGBA, unsigned int _globalFormat=GL_RGBA);
    ~ColorTexture();

    void bind();
    void unbind();
    int getWidth();
    
    // Needs to be public to be accessed by GL calls
    unsigned int gluid;
    unsigned int glunit;
   
    static const unsigned int LINEAR;
    static const unsigned int NEAREST;
    static const unsigned int MIPMAP;

private:
    static unsigned int unitCount;
    static float borderColor[];
    static float borderColorB[];

    unsigned int width;
    unsigned int minFilter;
    unsigned int magFilter;
    unsigned int format;
    unsigned int globalFormat;
};

}; // namespace gigapoint

#endif // TEXTURE_H
