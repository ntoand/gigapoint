#include "Points.h"

using namespace omega;

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
}

Points::~Points() {
}

void Points::draw(Shader* shader, ColorTexture* texture) {

	if(shader == NULL)
		return;

    shader->bind();
    texture->bind();

    glAlphaFunc(GL_GREATER, 0.1);
	glEnable(GL_ALPHA_TEST);
	if(oglError) return;

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexPointer(3, GL_FLOAT, 3*sizeof(float), (GLvoid*)0);
    glColorPointer(3, GL_FLOAT, 3*sizeof(float), (GLvoid*)(3*sizeof(float)));
	if(oglError) return;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
	if(oglError) return;
	
	shader->transmitUniform("uColorTexture", (int)0);

	glDrawArrays(GL_POINTS, 0, vertices.size()/3);

	glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    
    shader->unbind();
    texture->unbind();
}

void Points::findCollisionPoints(const omega::Ray& r, vector<float>& cp) {
	int numpoints = vertices.size() / 6;
	for(int i=0; i < numpoints; i+=6) {
		Vector3f pos = Vector3f(vertices[i], vertices[i+1], vertices[i+2]);
		std::pair<bool, omega::real> result = r.intersects(Sphere(pos, 0.2));
		if(result.first)
			cp.push_back((float)result.second);
	}
}