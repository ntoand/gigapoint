#include "Viewer.h"

#include <iostream>
using namespace omega;
using namespace std;

Viewer::Viewer(): checkCollision(false) {
	
}

Viewer::~Viewer() {
	delete shader;
}

void Viewer::init() {
	// Shader + material
	list<string> attributes;
	list<string> uniforms;
	attributes.clear(); uniforms.clear();
	//attributes.push_back("VertexPosition");
	//attributes.push_back("VertexColor");
	shader = new Shader("points");

#ifdef USE_GEOM
	shader->load("shaders/sphere", attributes, uniforms);
#else
	shader->load("shaders/simple", attributes, uniforms);
#endif

	points = new Points();
}

void Viewer::draw() {
	points->draw(shader);

	//draw ray line
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	Vector3f spos = ray.getOrigin() - 1*ray.getDirection();
	Vector3f epos = ray.getOrigin() + 1*ray.getDirection();
	glLineWidth(2.0); 
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(spos[0], spos[1], spos[2]);
	glVertex3f(epos[0], epos[1], epos[2]);
	//glVertex3f(-2, -2, -2);
	//glVertex3f(2, 2, 2);
	glEnd();

	if(checkCollision) {
		vector<float> cp;
		points->findCollisionPoints(ray, cp);
		cout << "Num coll points: " << cp.size()<< endl;
		for(int i=0; i < cp.size(); i++) {
			Vector3f pos = ray.getPoint(cp[i]);
			cout << cp[i] << " " << pos[0] << " " << pos[1] << " " << pos[2] << endl;;
		}
		checkCollision = false;
	}
}

void Viewer::setCheckCollision(bool c, const Ray& r) {
	checkCollision = c;
	ray = r;
}