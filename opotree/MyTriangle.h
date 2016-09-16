#ifndef DRAW_TRIANGLE_H
#define DRAW_TRIANGLE_H

#include <omega.h>
#include <omegaGl.h>

#include "Shader.h"

class MyTriangle {

private:
	GLuint vbo_triangle;
    GLuint program;
    float bbox[6];
    Shader* shader;

public:
	MyTriangle();
	~MyTriangle();
	int testFrustum(float V[6][4]);
	int initResources();
    void useProgram();
	void draw(const omega::DrawContext& context);
};

#endif
