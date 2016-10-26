#include "PointCloud.h"
#include "Utils.h"

#include <iostream>

using namespace std;
using namespace omicron;

PointCloud::PointCloud(string cfgfile, bool mas): master(mas) {

	// Option
	if(Utils::loadOption(cfgfile, option) != 0){
		cout << "Error: cannot load option " << endl;
		return;
	}
	if(master)
		Utils::printOption(option);

	// PC Info
	pcinfo = new PCInfo();
	if(Utils::loadPCInfo(option.dataDir, pcinfo) != 0) {
		cout << "Error: cannot load pc info" << endl;
		return;
	}
	if(master)
		Utils::printPCInfo(pcinfo);

	// Shader + material
	list<string> attributes;
	list<string> uniforms;
	attributes.clear(); uniforms.clear();
	attributes.push_back("VertexPosition");
	attributes.push_back("VertexColor");

	uniforms.push_back("uScreenHeight");
	uniforms.push_back("uSpacing");
	uniforms.push_back("uPointSize");
	uniforms.push_back("uPointScale");
	uniforms.push_back("uMinPointSize");
	uniforms.push_back("uMaxPointSize");

	Shader* shader = new Shader("point");
	shader->load("shaders/point", attributes, uniforms);

	material = new Material(shader);
	material->setPointSize(option.pointSize);
	material->setMaterial(option.material);
	material->setSizeType(option.sizeType);
	material->setQuality(option.quality);
	material->setScreenHeight(option.screenHeight);

	// root node
	string name = "r";
	root = new NodeGeometry(name);
	root->setInfo(pcinfo);
	if(root->loadHierachy()) {
		cout << "fail to load root hierachy" << endl;
		return;
	}
	if(root->loadData()) {
		cout << "fail to load root data " << endl;
		return;
	}

	preDisplayListSize = 0;

	numLoaderThread = option.numReadThread;
	// reading threads
	if(nodeLoaderThreads.size() == 0) {
    	for(int i = 0; i < numLoaderThread; i++) {
    		NodeLoaderThread* t = new NodeLoaderThread(nodeQueue);
    		t->start();
    		nodeLoaderThreads.push_back(t);
	    }
	
    }
    lrucache = new LRUCache(option.maxNodeInMem);

    preloadUpToLevel(option.preloadToLevel);
}

PointCloud::~PointCloud() {
	// desktroy tree
	if(pcinfo)
		delete pcinfo;
}

int PointCloud::preloadUpToLevel(const int level) {
	priority_queue<NodeWeight> priority_queue;
	priority_queue.push(NodeWeight(root, 1));

	unsigned numloaded = 0;

	cout << "Preload data to tree level " << level << " ..." << endl;

	while(priority_queue.size() > 0) {

		NodeGeometry* node = priority_queue.top().node;
    	priority_queue.pop();
		
		bool canload = false;
		if(numloaded + node->getNumPoints() < option.visiblePointTarget)
    		canload = true;

    	if(!canload)
    		continue;

		node->loadHierachy();
		node->loadData();
		lrucache->insert(node->getName(), node);

		if(node->getLevel() >= level)
			continue;

		for(int i=0; i < 8; i++) {
			if(node->getChild(i) == NULL)
				continue;
			priority_queue.push(NodeWeight(node->getChild(i), 1.0/node->getChild(i)->getLevel()));
		}

	}

	return 0;
}

int PointCloud::updateVisibility(const float MVP[16], const float campos[3]) {
	float V[6][4];
    Utils::getFrustum(V, MVP);
	
    priority_queue<NodeWeight> priority_queue;

    priority_queue.push(NodeWeight(root, 1));
    displayList.clear();
    numVisibleNodes = 0;
    numVisiblePoints = 0;

    unsigned int start_time = Utils::getTime();

    while(priority_queue.size() > 0){
    	NodeGeometry* node = priority_queue.top().node;
    	priority_queue.pop();
    	bool visible = false;
    	if(Utils::testFrustum(V, node->getBBox()) >= 0 && numVisiblePoints + node->getNumPoints() < option.visiblePointTarget)
    		visible = true;
	    
	    if(!visible)
	    	continue; 

	    numVisibleNodes++;
		numVisiblePoints += node->getNumPoints();

		node->loadHierachy();

		// add to load queue
		if(!node->inQueue() && node->canAddToQueue()) {
			node->setInQueue(true);
			nodeQueue.add(node);
		}
		
		displayList.push_back(node);
		lrucache->insert(node->getName(), node);

		if(Utils::getTime() - start_time > 150)
			return 0;
		
		// add children to priority_queue
		for(int i=0; i < 8; i++) {
			if(node->getChild(i) == NULL)
				continue;
			//calculte weight
			float* centre = node->getSphereCentre();
			float radius = node->getSphereRadius();
			float distance = Utils::distance(centre, campos);
			float fov = 0.6;
			float pr = 1 / tan(fov) * radius / sqrt(distance*distance - radius*radius);
			float weight = pr;
			if(distance - radius < 0)
				weight = FLT_MAX;

			float screenpixelradius = option.screenHeight * pr;
			if(screenpixelradius < option.minNodePixelSize)
				continue;

			priority_queue.push(NodeWeight(node->getChild(i), weight));
			//priority_queue.push(NodeWeight(node->getChild(i), 1.0/node->getChild(i)->getLevel()));
		}

    }

    return 0;
}

void PointCloud::draw() {

	for(list<NodeGeometry*>::iterator it = displayList.begin(); it != displayList.end(); it++) {
		NodeGeometry* node = *it;
		node->draw(material);
	}
}
