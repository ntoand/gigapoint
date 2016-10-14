#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Shader.h"

class Material {

private:
	Shader* shader;

    int material; // MATERIAL_RGB, MATERIAL_ELEVATION, MATERIAL_TREEDEPTH
	int quality; // QUALITY_SQUARE, QUALITY_CIRCLE
    int pointSize;
	int sizeType; // SIZE_FIXED, SIZE_ADAPTIVE
    int screenHeight;

public:
	Material(Shader* shader);

	Shader* getShader() {return shader;};
    Shader* bind();

    int getQuality() { return quality; }
    void setQuality( int q ) { quality = q; }
    int getPointSize() { return pointSize; }
    void setPointSize( int s ) { pointSize = s; }
    int getSizeType() { return sizeType; }
    void setSizeType( int s ) { sizeType = s; }
    int getMaterial() { return material; }
    void setMaterial( int m ) { material = m; }
    int getScreenHeight() { return screenHeight; }
    void setScreenHeight( int h ) { screenHeight = h; }
};

#endif