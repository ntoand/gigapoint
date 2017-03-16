#include "ColorTexture.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

namespace gigapoint {

const unsigned int ColorTexture::LINEAR = GL_LINEAR;
const unsigned int ColorTexture::NEAREST = GL_NEAREST;
const unsigned int ColorTexture::MIPMAP = GL_LINEAR_MIPMAP_LINEAR;

// for colormap
ColorTexture::ColorTexture(const char* filename, unsigned int _format, unsigned int _globalFormat) {
    FILE* fp;
    char* content = NULL; 
    fp = fopen( filename, "rb" );
    if(fp == NULL) {
        cout << "Error: cannot load file " << filename << endl;
        exit(0);
    }

    //load content (rgb1) 
    unsigned int length;   
    fseek( fp, 0, SEEK_END ); 
    length = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    content = new char [length];
    fread( content, sizeof( char ), length, fp );
    fclose( fp );
    width = length / 4;
    height = 1;

    //cout << "width: " << width << endl;
    //unsigned char* buff = (unsigned char*) content;
    //for(int i=0; i < length; i+=4)
    //    printf("%d %d %d %d %d\n", i / 4, buff[i], buff[i+1], buff[i+2], buff[i+3]);

    //init texture
    index = 0;
    gluid = 0;
    glunit = GL_TEXTURE0;
    minFilter = GL_NEAREST;
    magFilter = GL_NEAREST;
    format = _format;
    globalFormat = _globalFormat;
    glActiveTexture(glunit);
    glGenTextures(1, &gluid);
    glBindTexture(GL_TEXTURE_2D, gluid);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, 1, 0, globalFormat, GL_UNSIGNED_BYTE, content); //GL_UNSIGNED_INT_8_8_8_8_REV
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); //GL_CLAMP_TO_BORDER GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter); 
    glBindTexture(GL_TEXTURE_2D, 0);
}

// for framebuffer
ColorTexture::ColorTexture(unsigned int _index, unsigned int _width, unsigned int _height, unsigned int _format, unsigned int _globalFormat)
{
    index = _index;
    glunit = unitFromIndex(_index);
    gluid = 0;
    width = _width;
    height = _height;
    minFilter = GL_LINEAR;
    magFilter = GL_LINEAR;
    format = _format;
    globalFormat = _globalFormat;
    
    glActiveTexture(glunit);
    glGenTextures(1, &gluid);
    glBindTexture(GL_TEXTURE_2D, gluid);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, globalFormat, GL_UNSIGNED_BYTE, NULL);
    if(format == GL_DEPTH_COMPONENT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColorB);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    glActiveTexture(GL_TEXTURE0);
}


ColorTexture::~ColorTexture() {
    glDeleteTextures(1, &gluid);
}

void ColorTexture::bind() {
    glActiveTexture(glunit);
    glBindTexture(GL_TEXTURE_2D, gluid);
}

void ColorTexture::unbind() {
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int ColorTexture::getWidth() {
    return width;
}

void ColorTexture::resize(unsigned int _width, unsigned int _height)
{
    width = _width;
    height = _height;

    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, globalFormat, GL_UNSIGNED_BYTE, NULL);
}

// static
unsigned int ColorTexture::unitCount = 0;
float ColorTexture::borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
float ColorTexture::borderColorB[] = {0.0f, 0.0f, 0.0f, 0.0f};

void ColorTexture::resetUnit(int textureUnitOffset)
{
    unitCount = textureUnitOffset;
}

ColorTexture* ColorTexture::newFromNextUnit(unsigned int _width, unsigned int _height, unsigned int _format, unsigned int _globalFormat)
{
    return new ColorTexture(unitCount++, _width, _height, _format, _globalFormat);
}

unsigned int ColorTexture::unitFromIndex(unsigned int index)
{
    switch(index)
    {
        case 1: return GL_TEXTURE1;
        case 2: return GL_TEXTURE2;
        case 3: return GL_TEXTURE3;
        case 4: return GL_TEXTURE4;
        case 5: return GL_TEXTURE5;
        case 6: return GL_TEXTURE6;
        case 7: return GL_TEXTURE7;
        case 8: return GL_TEXTURE8;
        case 9: return GL_TEXTURE9;
        default: return GL_TEXTURE0;
    }
}

}; //namespace gigapoint
