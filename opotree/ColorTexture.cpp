#include "ColorTexture.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

const unsigned int ColorTexture::LINEAR = GL_LINEAR;
const unsigned int ColorTexture::NEAREST = GL_NEAREST;
const unsigned int ColorTexture::MIPMAP = GL_LINEAR_MIPMAP_LINEAR;

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

    //cout << "width: " << width << endl;
    //unsigned char* buff = (unsigned char*) content;
    //for(int i=0; i < length; i+=4)
    //    printf("%d %d %d %d %d\n", i / 4, buff[i], buff[i+1], buff[i+2], buff[i+3]);

    //init texture
    gluid = 0;
    glunit = GL_TEXTURE0;
    minFilter = GL_LINEAR;
    magFilter = GL_LINEAR;
    format = _format;
    globalFormat = _globalFormat;
    glActiveTexture(glunit);
    glGenTextures(1, &gluid);
    glBindTexture(GL_TEXTURE_1D, gluid);
    glTexImage1D(GL_TEXTURE_1D, 0, format, width, 0, globalFormat, GL_UNSIGNED_BYTE, content); //GL_UNSIGNED_INT_8_8_8_8_REV
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); //GL_CLAMP_TO_BORDER GL_REPEAT
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, magFilter); 
}

ColorTexture::~ColorTexture() {
    glDeleteTextures(1, &gluid);
}

void ColorTexture::bind() {
    glActiveTexture(glunit);
    glBindTexture(GL_TEXTURE_1D, gluid);
}

void ColorTexture::unbind() {
    //glActiveTexture(GL_TEXTURE0);
}

int ColorTexture::getWidth() {
    return width;
}
