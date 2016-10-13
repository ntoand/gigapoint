#ifndef _POINTCLOUD_H_
#define _POINTCLOUD_H_

#include <vector>
#include <list>

#include <omicron/Thread.h>

#include "NodeGeometry.h"
#include "Material.h"
#include "LRU.h"

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

class PointCloud {
private:
	PCInfo* pcinfo;
	Material* material;
	NodeGeometry* root;
	list<NodeGeometry*> displayList;
	int preDisplayListSize;

	Option option;
	int numVisibleNodes;
	unsigned int numVisiblePoints;

	static list<omicron::Thread*> sNodeLoaderThread;
	static int sNumLoaderThreads;

	LRUCache* lrucache;

public:
	PointCloud(string cfgfile);
	~PointCloud();

	int updateVisibility(const float MVP[16], const float campos[3]);
	void draw(const float MVP[16]);
};

#endif