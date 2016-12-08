#include "Viewer.h"

#include <iostream>
using namespace omega;
using namespace std;

Viewer::Viewer(): checkCollision(false) {
	
}

Viewer::~Viewer() {
	delete shader;
	delete texture;
}

void Viewer::init() {
	// Shader + material
	list<string> attributes;
	list<string> uniforms;
	attributes.clear(); uniforms.clear();
	//attributes.push_back("VertexPosition");
	//attributes.push_back("VertexColor");
	//uniforms.push_back("uColorTexture");
	shader = new Shader("points");

#ifdef USE_GEOM
	shader->load("shaders/simple", attributes, uniforms);
#else
	shader->load("shaders/simple", attributes, uniforms);
#endif

	texture = new ColorTexture("colortable.dat");

	points = new Points();
}

void Viewer::draw() {
	points->draw(shader, texture);

	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	//draw axes
	glLineWidth(1.0);
	glBegin(GL_LINES);
	
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0, 0, 0);
	glVertex3f(5, 0, 0);

	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 5, 0);

	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 5);

	glEnd();


	//draw ray line
	Vector3f spos = ray.getOrigin(); //- 1*ray.getDirection();
	Vector3f epos = ray.getOrigin() + 10*ray.getDirection();
	glLineWidth(2.0); 
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(spos[0], spos[1], spos[2]);
	glVertex3f(epos[0], epos[1], epos[2]);
	glEnd();

	if(checkCollision) {
		vector<float> cp;
		points->findCollisionPoints(ray, cp);
		cout << "Num coll points: " << cp.size()<< endl;
		for(int i=0; i < cp.size(); i++) {
			Vector3f pos = ray.getPoint(cp[i]);
			cout << cp[i] << " " << pos[0] << " " << pos[1] << " " << pos[2] << endl;
		}
		checkCollision = false;

		if(cp.size() > 0) {
			Vector3f pos = ray.getPoint(cp[0]);
			collisionPoints.push_back(pos[0]);
			collisionPoints.push_back(pos[1]);
			collisionPoints.push_back(pos[2]);
		}
	}

	// draw collision points
	glEnable(GL_PROGRAM_POINT_SIZE_EXT);
    glPointSize(20);
    glColor3f(0.0, 1.0, 0.0);
    int numpoints = collisionPoints.size() / 3;
    glBegin(GL_POINTS);
    for(int i=0; i < numpoints; i++)
		glVertex3f(collisionPoints[3*i+0], collisionPoints[3*i+1], collisionPoints[3*i+2]);
	glEnd();
}

void Viewer::setCheckCollision(bool c, const Ray& r) {
	checkCollision = c;
	ray = r;
}