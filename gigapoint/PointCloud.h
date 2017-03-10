#ifndef _POINTCLOUD_H_
#define _POINTCLOUD_H_

#include <vector>
#include <list>

#include "NodeGeometry.h"
#include "Material.h"
#include "LRU.h"
#include "Thread.h"
#include "wqueue.h"
#include "FrameBuffer.h"

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
            	node->loadData();
        }
        return NULL;
    }
};


// PointCloud class
class PointCloud {
private:
	bool master;

	int width, height;
	
	PCInfo* pcinfo;
	Material* materialPoint;
	Material* materialEdl;
	FrameBuffer* frameBuffer;
	NodeGeometry* root;
	std::list<NodeGeometry*> displayList;
	int preDisplayListSize;
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

	// interaction
	void setInteractMode(int v) { interactMode = v; }
	int getInteractMode() { return interactMode; }
	void updateRay(const omega::Ray& r);
	void findHitPoint();
};

}; //namespace gigapoint

#endif
