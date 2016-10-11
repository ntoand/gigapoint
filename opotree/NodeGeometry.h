#ifndef _NODE_GEOMETRY_H_
#define _NODE_GEOMETRY_H_

#include "Util.h"
#include "PCLoader.h"
#include "Shader.h"

#include <string>
#include <vector>

using namespace std;

struct HRC_Item {
	string name;
	unsigned char children;
	unsigned int numpoints;
	HRC_Item(string n, unsigned char c, unsigned int num){
		name = n;
		children = c;
		numpoints = num;
	}
};

class NodeGeometry {

private:
	int id;
	string name;
	int index;
	float bbox[6];
	float tightbbox[6];
	float spherecentre[3];
	float sphereradius;
	int numpoints;
	int level;

	bool loaded;
	bool initvbo;
	bool visible;

	//data
	vector<float> vertices;
	vector<float> colors;
	unsigned int vertexbuffer;
	unsigned int colorbuffer;
	Shader* shader;

	NodeGeometry* parent;
	NodeGeometry* children[8];
	bool haschildren;
	string datafile;

public:
	NodeGeometry(string name);
	~NodeGeometry();

	void setIndex(int ind) { index = ind; }
	int getIndex() { return index; }
	void setLevel(int l) { level = l; }
	int getLevel() { return level; }
	void setNumPoints(int n) { numpoints = n; }
	int getNumPoints() { return numpoints; }
	void setHasChildren(bool v) { haschildren = v; }
	bool hasChildren() { return haschildren; }
	float* getSphereCentre() { return spherecentre; }
	float getSphereRadius() { return sphereradius; }

	void setParent(NodeGeometry* p) { parent = p;}
	void addChild(NodeGeometry* c) { children[c->getIndex()] = c; }
	NodeGeometry* getChild(int i) { return children[i]; }

	string getName() { return name; }
	bool isLoaded() { return loaded; }
	void setVisible(const bool v) { visible = v; }
	bool isVisible() { return visible; }

	void setBBox(const float* bbox);
	float* getBBox() { return bbox; }

	void addPoint(float x, float y, float z);
	void addColor(float r, float g, float b);
	string getHierarchyPath(const PCInfo& info);
	int loadData(const PCInfo& info, bool movetocentre = true);
	void printInfo();
	int initVBO();
	void draw(Shader* sh);
};

#endif