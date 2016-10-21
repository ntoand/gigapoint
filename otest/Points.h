#ifndef _CUBE_H_
#define _CUBE_H_

#include <omega.h>
#include <omegaGl.h>

#include <vector>

#include "Shader.h"

class Points {

private:
	unsigned int vertexbuffer;
	unsigned int colorbuffer;

	Shader* shader;

	std::vector<float> vertices;
	std::vector<float> colors;
public:
	Points();
	~Points();
	void draw();
};

#endif