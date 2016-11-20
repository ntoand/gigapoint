#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Shader.h"
#include "ColorTexture.h"

class Material {

private:
	Shader* shader;
    Option* option;
    ColorTexture* texture;

    std::list<std::string> attributes;
	std::list<std::string> uniforms;

public:
	Material(Option* option);

	Shader* getShader() { return shader; };
    Shader* bind();
    Option* getOption() { return option; }
    ColorTexture* getColorTexture() { return texture; }
    void reloadShader();
};

#endif
