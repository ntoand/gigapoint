#include "Points.h"

float randomValue() {
    return ((double) rand() / (RAND_MAX));
}

Points::Points() {

    //generate random points
    int numpoints = 200;
    for(int i=0; i < numpoints; i++) {
        vertices.push_back(randomValue()); vertices.push_back(randomValue()); vertices.push_back(randomValue());
        colors.push_back(randomValue()); colors.push_back(randomValue()); colors.push_back(randomValue());
    }

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*4, &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size()*4, &colors[0], GL_STATIC_DRAW);


	// Shader + material
	list<string> attributes;
	list<string> uniforms;
	attributes.clear(); uniforms.clear();
	attributes.push_back("VertexPosition");
	attributes.push_back("VertexColor");
	shader = new Shader("simple");
	shader->load("shaders/simple", attributes, uniforms);
}

Points::~Points() {
	delete shader;
}

void Points::draw() {

    shader->bind();

    unsigned int att = shader->attribute("VertexPosition");
	glEnableVertexAttribArray(att);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
	   att,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);

    att = shader->attribute("VertexColor");
	glEnableVertexAttribArray(att);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
	   att,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	
	glDrawArrays(GL_POINTS, 0, vertices.size()/3);
}
