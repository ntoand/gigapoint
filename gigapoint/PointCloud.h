#ifndef _POINTCLOUD_H_
#define _POINTCLOUD_H_

#include <vector>
#include <list>

#include "NodeGeometry.h"
#include "Material.h"
#include "LRU.h"
#include "Thread.h"
#include "wqueue.h"

namespace gigapoint {

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
	PCInfo* pcinfo;
	Material* material;
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

	// interaction
	int interactMode;
	omega::Ray ray;
	vector<HitPoint*> hitPoints; 

    map<string, NodeGeometry*> *nodes;
    void debug();
    void reload();
    void unload();



public:
	PointCloud(Option* option, bool master = false);
	~PointCloud();
	int initPointCloud();
	void setReloadShader(bool b) { needReloadShader = b; }
	void setPrintInfo(bool b) { printInfo = b; }

	int preloadUpToLevel(const int level=0);
	int updateVisibility(const float MVP[16], const float campos[3]);
	void draw();


	// interaction
	void setInteractMode(int v) { interactMode = v; }
	int getInteractMode() { return interactMode; }
	void updateRay(const omega::Ray& r);
	void findHitPoint();

    void setReloading(bool b) {fullReload=b;}
    void setUnloading(bool b) {_unload=b;}
    void togglePauseUpdate() {pauseUpdate = !pauseUpdate;}
    void resetRootHierarchy();
    void flagNodeAsDirty(const std::string& text);

};

}; //namespace gigapoint

#endif
