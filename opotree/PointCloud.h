#ifndef _POINTCLOUD_H_
#define _POINTCLOUD_H_

#include <vector>
#include <list>

#include "NodeGeometry.h"
#include "Material.h"

typedef struct Option_t {
	unsigned int visiblePointTarget;
	int minNodePixelSize;
} Option;

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
	PCInfo pcinfo;
	Material* material;
	NodeGeometry* root;
	list<NodeGeometry*> displayList;

	Option option;

public:
	PointCloud(string datadir);
	~PointCloud();

	int updateVisibility(const float MVP[16]);
	void draw(const float MVP[16]);
};

#endif