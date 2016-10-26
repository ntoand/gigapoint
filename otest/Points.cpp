#include "Points.h"

float randomValue() {
    return ((double) rand() / (RAND_MAX));
}

Points::Points() {

    //generate random points
    int numpoints = 200;
    for(int i=0; i < numpoints; i++) {
        vertices.push_back(randomValue()); vertices.push_back(randomValue()); vertices.push_back(randomValue());
        vertices.push_back(randomValue()); vertices.push_back(randomValue()); vertices.push_back(randomValue());
    }

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*4, &vertices[0], GL_STATIC_DRAW);

	// Shader + material
	list<string> attributes;
	list<string> uniforms;
	attributes.clear(); uniforms.clear();
	//attributes.push_back("VertexPosition");
	//attributes.push_back("VertexColor");
	shader = new Shader("simple");
	shader->load("shaders/simple", attributes, uniforms);
}

Points::~Points() {
	delete shader;
}

void Points::draw() {

    shader->bind();

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexPointer(3, GL_FLOAT, 3*sizeof(float), (GLvoid*)0);
    glColorPointer(3, GL_FLOAT, 3*sizeof(float), (GLvoid*)(3*sizeof(float)));

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
	
	glDrawArrays(GL_POINTS, 0, vertices.size()/3);

	glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}
