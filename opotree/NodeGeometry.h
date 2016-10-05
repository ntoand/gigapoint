#ifndef _NODE_GEOMETRY_H_
#define _NODE_GEOMETRY_H_

#include "Util.h"
#include "PCLoader.h"
#include "Shader.h"

#include <string>
#include <vector>

using namespace std;

class NodeGeometry {

private:
	int id;
	string name;
	int index;
	float bbox[6];
	int numpoints;
	int level;

	bool loaded;
	bool initvbo;
	bool visible;

	//data
	vector<float> vertices;
	vector<float> colors;
	unsigned int vbo;
	Shader* shader;

public:
	NodeGeometry* parent;
	NodeGeometry* children[8];

public:
	NodeGeometry(string name);
	~NodeGeometry();

	string getName() { return name; }
	int getNumPoints() { return numpoints; }
	bool isLoaded() { return loaded; }
	void setVisible(const bool v) { visible = v; }
	bool isVisible() { return visible; }
	float* getBBox() { return bbox; }

	void addPoint(float x, float y, float z);
	void addColor(float r, float g, float b);
	int loadBinData(const PCInfo info, bool plusbbmin = false);
	void printInfo();
	int initVBO(Shader* sh);
	void draw();
};

#endif