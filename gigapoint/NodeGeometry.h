#ifndef _NODE_GEOMETRY_H_
#define _NODE_GEOMETRY_H_

#include "Utils.h"
#include "Material.h"

#include <string>
#include <vector>
#include <fstream>

//using namespace std;

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

class LRUCache;

class NodeGeometry {

private:
    //int id; //dead variable ?
	string name;
	int index;
	float bbox[6];
	float tightbbox[6];
	//float tightbbox[6];
	float spherecentre[3];
	float sphereradius;
	int numpoints;
	int level;
    bool initvbo;
    //bool visible;
	bool hierachyloaded;
	bool inqueue;
	bool loading;
	bool loaded;

    bool isupdating; // currently updating similar to isloading
    bool dirty; // marked for update, similar to inqueue

    string hrc_filename;
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
    NodeGeometry* updateCache;

	bool haschildren;
	string datafile;
    bool updateFinished() {
        if (updateCache != NULL)
            if (updateCache->isLoaded())
                return true;
        return false;
    }

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

    bool inQueue() { return inqueue; }
    bool canAddToQueue() { return (!loading && !isLoaded()); }

	void setInfo(PCInfo* in) { info = in; }
	PCInfo* getInfo() { return info; }

	void setParent(NodeGeometry* p) { parent = p;}
	void addChild(NodeGeometry* c) { children[c->getIndex()] = c; }
	NodeGeometry* getChild(int i) { return children[i]; }

	string getName() { return name; }
    bool isLoaded()  { return loaded; }
    //void setVisible(const bool v) {visible = v; }
    //bool isVisible() { return visible; }

	void setBBox(const float* bbox);
	float* getBBox() { return bbox; }
	void setTightBBox(const float* bbox);
	float* getTightBBox() { return tightbbox; }

	void addPoint(float x, float y, float z);
	void addColor(float r, float g, float b);
	string getHierarchyPath();
    int loadHierachy(LRUCache* nodecache,bool force=false);
    bool canLoadHierarchy() {return (level % info->hierarchyStepSize) == 0;}
	int loadData();
	void printInfo();
	int initVBO();
	void draw(Material* material, const int height);
    void freeData(bool keepupdatecache=false);

	//interaction
	void findHitPoint(const omega::Ray& r, HitPoint* point);

    //tracer
    Point getPoint(PointIndex ind);
    std::vector< Point > getPointsInSphericalNeighbourhood(Point current, float search_r, int depthLevel);
    void getPointData(Point &point);
    void setPointColor(Point &point,int r,int g,int b);

    //onlineUpdate
    bool isDirty() {return dirty;}
    void setDirty() {dirty=true;}
    bool isUpdating() {return isupdating;}
    void initUpdateCache();
    NodeGeometry* getUpdateCache() {return updateCache;}
    void Update();

    void test();

};

}; //namespace gigapoint

#endif
