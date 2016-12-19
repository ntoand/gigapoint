#ifndef _NODE_GEOMETRY_H_
#define _NODE_GEOMETRY_H_

#include "Utils.h"
#include "Material.h"

#include <string>
#include <vector>
#include <fstream>

using namespace std;

namespace gigapoint {

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
	//float tightbbox[6];
	float spherecentre[3];
	float sphereradius;
	int numpoints;
	int level;

	bool hierachyloaded;
	bool inqueue;
	bool loading;
	bool loaded;
	bool initvbo;
	bool visible;
        ifstream::pos_type filesize;
        ifstream::pos_type hrcfilesize;
	PCInfo* info;
        ifstream::pos_type getFilesize(const char* filename);

	//data
	vector<float> vertices;
	vector<unsigned char> colors;
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
	void setInQueue(bool b) { inqueue = b; }
	float inQueue() { return inqueue; }
        bool canAddToQueue() { return (!loading && !isLoaded()); }

	void setInfo(PCInfo* in) { info = in; }
	PCInfo* getInfo() { return info; }

	void setParent(NodeGeometry* p) { parent = p;}
	void addChild(NodeGeometry* c) { children[c->getIndex()] = c; }
	NodeGeometry* getChild(int i) { return children[i]; }

	string getName() { return name; }
        bool isLoaded();
	void setVisible(const bool v) { visible = v; }
	bool isVisible() { return visible; }

	void setBBox(const float* bbox);
	float* getBBox() { return bbox; }
	void setTightBBox(const float* bbox);
	float* getTightBBox() { return tightbbox; }

	void addPoint(float x, float y, float z);
	void addColor(float r, float g, float b);
	string getHierarchyPath();
	int loadHierachy();
	int loadData();
	void printInfo();
	int initVBO();
	void draw(Material* material);

	void freeData();

	//interaction
	void findHitPoint(const omega::Ray& r, HitPoint* point);
};

}; //namespace gigapoint

#endif
