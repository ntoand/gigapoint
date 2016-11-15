#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Shader.h"

class Material {

private:
	Shader* shader;

    int material; // MATERIAL_RGB, MATERIAL_ELEVATION, MATERIAL_TREEDEPTH
	int quality; // QUALITY_SQUARE, QUALITY_CIRCLE
	int sizeType; // SIZE_FIXED, SIZE_ADAPTIVE
    float pointSize;
    float screenHeight;
    float scaleXYZ[3];

public:
	Material(Shader* shader);

	Shader* getShader() {return shader;};
    Shader* bind();

    int getQuality() { return quality; }
    void setQuality( int q ) { quality = q; }
    int getSizeType() { return sizeType; }
    void setSizeType( int s ) { sizeType = s; }
    int getMaterial() { return material; }
    void setMaterial( int m ) { material = m; }
    float getPointSize() { return pointSize; }
    void setPointSize( float s ) { pointSize = s; }
    float getScreenHeight() { return screenHeight; }
    void setScreenHeight( float h ) { screenHeight = h; }
    void setScaleXYZ( float s[3] );
    float getScale(int i = 0) { return scaleXYZ[i]; }
};

#endif