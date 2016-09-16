#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Shader.h"

class Material {

private:
	Shader* shader;

public:
	Material(Shader* shader);

	Shader* getShader() {return shader;};

    Shader* bind();
};

#endif