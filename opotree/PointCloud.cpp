#include "PointCloud.h"
#include "Utils.h"

#include <iostream>

using namespace std;
using namespace omicron;

Lock sNodeQueueLock;
bool sShutdownLoaderThread = false;
int PointCloud::sNumLoaderThreads = 1;
list<Thread*> PointCloud::sNodeLoaderThread;
list<NodeGeometry*> sNodeQueue;

class NodeLoaderThread: public Thread
{
public:
    NodeLoaderThread()
    {}

    virtual void threadProc()
    {
        cout << "NodeLoaderThread: start" << endl;

        while(!sShutdownLoaderThread)
        {
          	if(sNodeQueue.size() > 0)
          	{
          		sNodeQueueLock.lock();
          		if(sNodeQueue.size() > 0)
                {
                	NodeGeometry* node = sNodeQueue.front();
                    sNodeQueue.pop_front();
                    sNodeQueueLock.unlock();
                    //cout << "queue size: " << sNodeQueue.size() << endl;
                    node->loadData();
                    node->setInQueue(false);
                }
                else
                {
                	sNodeQueueLock.unlock();
                }

          	}
            osleep(1);
        }
        cout << "NodeLoaderThread: shutdown" << endl;
    }
};


PointCloud::PointCloud(string cfgfile) {

	// Option
	if(Utils::loadOption(cfgfile, option) != 0){
		cout << "Error: cannot load option " << endl;
		return;
	}
	Utils::printOption(option);

	// PC Info
	pcinfo = new PCInfo();
	if(Utils::loadPCInfo(option.dataDir, pcinfo) != 0) {
		cout << "Error: cannot load pc info" << endl;
		return;
	}
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

	sNumLoaderThreads = option.numReadThread;

	// reading threads
	if(sNodeLoaderThread.size() == 0)
    {
    	for(int i = 0; i < sNumLoaderThreads; i++)
	    {
	        Thread* t = new NodeLoaderThread();
	     	t->start();
	        sNodeLoaderThread.push_back(t);;
	    }
    }

    lrucache = new LRUCache(option.maxNodeInMem);
}

PointCloud::~PointCloud() {
	// desktroy tree
	if(pcinfo)
		delete pcinfo;

	sShutdownLoaderThread = true;
}

int PointCloud::updateVisibility(const float MVP[16], const float campos[3]) {
	float V[6][4];
    Utils::getFrustum(V, MVP);

    priority_queue<NodeWeight> priority_queue;

    priority_queue.push(NodeWeight(root, 1));
    displayList.clear();
    numVisibleNodes = 0;
    numVisiblePoints = 0;

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
		if(!node->inQueue() && !node->isLoaded()) {
			node->setInQueue(true);
			sNodeQueueLock.lock();
			sNodeQueue.push_back(node);
			sNodeQueueLock.unlock();
		}

		displayList.push_back(node);
		lrucache->insert(node->getName(), node);
		
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

    /*
    if(displayList.size() != preDisplayListSize) {
    	preDisplayListSize = displayList.size();
    	cout << "# vis nodes: " << displayList.size() << " # points: " << numVisiblePoints << endl;
    	cout << "nodes: " << endl;
    	for(list<NodeGeometry*>::iterator it = displayList.begin(); it != displayList.end(); it++) {
			NodeGeometry* node = *it;
			//cout << node->getName() << " ";
			node->printInfo();
		}
		cout << endl;
		lrucache->dumpDebug();
    }
    */

    return 0;
}

void PointCloud::draw(const float MVP[16]) {

	for(list<NodeGeometry*>::iterator it = displayList.begin(); it != displayList.end(); it++) {
		NodeGeometry* node = *it;
		node->draw(material);
	}
}
