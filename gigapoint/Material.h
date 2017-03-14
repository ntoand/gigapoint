#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Shader.h"
#include "ColorTexture.h"

namespace gigapoint {

class Material {

protected:
	Shader* shader;
    Option* option;
    
    std::string name;
    std::string shaderstr;

    std::list<std::string> attributes;
	std::list<std::string> uniforms;

public:
	Material(Option* option);

	Shader* getShader() { return shader; };
    Shader* bind();
    Option* getOption() { return option; }
    void reloadShader();
};



class MaterialPoint : public Material {

protected:
    ColorTexture* texture;

public:
    MaterialPoint(Option* option);
    ColorTexture* getColorTexture() { return texture; }
};

#define NUM_NEIGHBOURS 4
class MaterialEdl : public Material {

protected:
    float neighbours[NUM_NEIGHBOURS*2];

public:
    MaterialEdl(Option* option);
    float* getNeighbours() { return neighbours; }

};


}; //namespace gigapoint

#endif
