#ifndef _POINTCLOUD_H_
#define _POINTCLOUD_H_

#include "NodeGeometry.h"
#include "Material.h"
#include "LRU.h"
#include "Thread.h"
#include "wqueue.h"
#include "FrameBuffer.h"

namespace gigapoint {

#include <vector>
#include <list>


using namespace tthread;

struct NodeWeight {
	NodeGeometry* node;
	float weight;
	
	NodeWeight(NodeGeometry* n, float w) {
		node = n;
		weight = w;
	}

	bool operator<(const NodeWeight& nw) const {
        return weight < nw.weight;
    }
};

class NodeLoaderThread: public Thread {    
private:
	wqueue<NodeGeometry*>& m_queue;
	int maxLoadSize;

public:
	NodeLoaderThread(wqueue<NodeGeometry*>& queue, int m) : m_queue(queue), maxLoadSize(m) {}

	void* run() {
        for (;;) {
            NodeGeometry* node = (NodeGeometry*)m_queue.remove();
	    node->setInQueue(false);        
	    if(m_queue.size() < maxLoadSize)
            if(!node->isDirty()) {
                node->loadData();
            } else {
                node->initUpdateCache();
                //node->updateCache->loadHierachy(); // called during update visibility
                node->getUpdateCache()->loadData();
            }
        }
        return NULL;
    }
};


// PointCloud class
class PointCloud {
private:
	bool master;

    bool pauseUpdate; //if true do not updateVisibility
    bool fullReload; // prevents updates and rendering while reloading
    bool _unload;
    bool render;
	int width, height;	

	PCInfo* pcinfo;
	Material* materialPoint;
	Material* materialEdl;
	FrameBuffer* frameBuffer;
	NodeGeometry* root;
	std::list<NodeGeometry*> displayList;
    //int preDisplayListSize;
	bool needReloadShader;
	bool printInfo;

	Option* option;
	int numVisibleNodes;
	unsigned int numVisiblePoints;

	// loader threads
	wqueue<NodeGeometry*>  nodeQueue;
	std::list<NodeLoaderThread*> nodeLoaderThreads;
	int numLoaderThread;

	// cache 
	LRUCache* lrucache;

    //map<string, NodeGeometry*> *nodes;
    void debug();
    void reload();
    void unload();

	// draw quad
	unsigned int quadVao;
	unsigned int quadVbo;

private:
	void initMaterials();


public:
	PointCloud(Option* option, bool master = false);
	~PointCloud();
	int initPointCloud();
	void setReloadShader(bool b) { needReloadShader = b; }
	void setPrintInfo(bool b) { printInfo = b; }

	int preloadUpToLevel(const int level=0);
	int updateVisibility(const float MVP[16], const float campos[3], const int width, const int height);
	void draw();
	void drawViewQuad();

    void setReloading(bool b) {fullReload=b;}
    void setUnloading(bool b) {_unload=b;}
    void togglePauseUpdate() {pauseUpdate = !pauseUpdate;}
    void resetRootHierarchy();
    bool flagNodeAsDirty(const std::string& text);

    PCInfo* getPCInfo(){return pcinfo;}

    Point getPointFromIndex(const PointIndex_ &index);
    std::vector< Point > getPointsInSphericalNeighbourhood(Point current, float search_r)
    {
        return root->getPointsInSphericalNeighbourhood(current,search_r);
    }

    //interface facade
    bool tryGetNode(const string &name,NodeGeometry*& node) {
        //lrucache->dumpDebug(std::coulrucache->tryGet(name,node);t);
        bool b = lrucache->tryGet(name,node);
        //return lrucache->tryGet(name,node);
        if (!b)
            std::cout << "unable to find node " << name << std::endl;
        else
            std::cout << "found node " << name << node->getName()<< std::endl;
        return b;
    }
    NodeGeometry* getRoot(){ return root;}
    std::list<NodeGeometry*> & getListOfVisibleNodesRef() {return displayList;}

    int getWidth() {return width;}
    int getHeight() {return height;}

}; //class PointCloud
}; //namespace gigapoint

#endif
