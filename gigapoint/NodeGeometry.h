#ifndef _NODE_GEOMETRY_H_
#define _NODE_GEOMETRY_H_

#include "Utils.h"
#include "Material.h"
#include "LRU.h"

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

enum LoadState {
	STATE_NONE = 0,
	STATE_INQUEUE,
	STATE_LOADING,
	STATE_LOADED
};

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
    
    LoadState loadstate;

    bool updating; // currently updating similar to isloading
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
    
private:
    void getRangeInfo(const Option* option, float &min, float &max, float &range);

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
    
    void setState(LoadState s) { loadstate = s; }
    bool inQueue() { return loadstate == STATE_INQUEUE; }
    bool canAddToQueue() { return loadstate == STATE_NONE; }
    bool isLoading() { return loadstate == STATE_LOADING; }
    bool isLoaded()  { return loadstate == STATE_LOADED; }

	void setInfo(PCInfo* in) { info = in; }
	PCInfo* getInfo() { return info; }

	void setParent(NodeGeometry* p) { parent = p;}
	void addChild(NodeGeometry* c) { children[c->getIndex()] = c; }
	NodeGeometry* getChild(int i) { return children[i]; }

	string getName() { return name; }
    
    //void setVisible(const bool v) {visible = v; }
    //bool isVisible() { return visible; }

	void setBBox(const float* bbox);
	float* getBBox() { return bbox; }
	void setTightBBox(const float* bbox);
	float* getTightBBox() { return tightbbox; }

	void addPoint(float x, float y, float z);
	void addColor(float r, float g, float b);
	string getHierarchyPath();
    int loadHierachy(LRUCache* lrucache, bool force=false);
    bool canLoadHierarchy() {return (level % info->hierarchyStepSize) == 0;}
	int loadData();
	void printInfo();
	int initVBO();
#ifdef STANDALONE_APP
	void draw(const float MV[16], const float MVP[16], Material* material, const int height);
#else
    void draw(Material* material, const int height);
#endif
    void freeData(bool keepupdatecache=false);

	//interaction
#ifndef STANDALONE_APP
	void findHitPoint(const omega::Ray& r, HitPoint* point);
#endif
    //tracer
    Point getPoint(PointIndex ind);
    std::vector< Point > getPointsInSphericalNeighbourhood(Point current, float search_r);
    void getPointData(Point &point);
    void setPointColor(Point &point,int r,int g,int b);

    //onlineUpdate
    bool isDirty() {return dirty;}
    void setDirty() {dirty=true;}
    bool isUpdating() {return updating;}
    void initUpdateCache();
    NodeGeometry* getUpdateCache() {return updateCache;}
    void Update();

};

}; //namespace gigapoint

#endif
