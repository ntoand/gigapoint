#include "Points.h"

using namespace omega;

float randomPosition() {
	return ((double) rand() / (RAND_MAX)) * 2 - 1;
}

unsigned char randomColor() {
    return 255*((double) rand() / (RAND_MAX));
}

Points::Points() {

    //generate random points
    int numpoints = 100;
    vertices.resize(numpoints*3);
    colors.resize(numpoints*3);
    for(int i=0; i < numpoints; i++) {
    	vertices[3*i] = randomPosition(); vertices[3*i+1] = randomPosition(); vertices[3*i+2] = randomPosition();
    	colors[3*i] = randomColor(); colors[3*i+1] = randomColor(); colors[3*i+2] = randomColor();
    }
    vertices[0] = vertices[1] = vertices[2] = 0;
    colors[0] = colors[1] = colors[2] = 255;

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(unsigned char), &colors[0], GL_STATIC_DRAW);
}

Points::~Points() {
}

void Points::draw(Shader* shader, ColorTexture* texture) {

	if(shader == NULL)
		return;

    shader->bind();
    //texture->bind();

    glAlphaFunc(GL_GREATER, 0.1);
	glEnable(GL_ALPHA_TEST);
	if(oglError) return;

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexPointer(3, GL_FLOAT, 3*sizeof(float), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glColorPointer(3, GL_UNSIGNED_BYTE, 3*sizeof(unsigned char), (GLvoid*)0);
	if(oglError) return;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
	if(oglError) return;
	
	//shader->transmitUniform("uColorTexture", (int)0);

	glDrawArrays(GL_POINTS, 0, vertices.size()/3);

	glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    
    shader->unbind();
    //texture->unbind();
}

void Points::findCollisionPoints(const omega::Ray& r, vector<float>& cp) {
	int numpoints = vertices.size() / 3;
	for(int i=0; i < numpoints; i++) {
		Vector3f pos = Vector3f(vertices[3*i], vertices[3*i+1], vertices[3*i+2]);
		std::pair<bool, omega::real> result = r.intersects(Sphere(pos, 0.01));
		if(result.first)
			cp.push_back((float)result.second);
	}
}
