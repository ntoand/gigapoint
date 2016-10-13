#ifndef _CUBE_H_
#define _CUBE_H_

#include <omega.h>
#include <omegaGl.h>

#include "Shader.h"

class Cube {

private:
	unsigned int vertexbuffer;
	unsigned int colorbuffer;

	Shader* shader;
public:
	Cube();
	~Cube();
	void draw();
};

#endif