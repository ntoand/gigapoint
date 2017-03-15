#include "PointCloud.h"
#include "Utils.h"

#include <iostream>

using namespace std;
using namespace omicron;

namespace gigapoint {

PointCloud::PointCloud(Option* opt, bool mas): option(opt), master(mas), pauseUpdate(false),fullReload(false),
                                                material(NULL),lrucache(NULL),_unload(false),render(true),
                                                needReloadShader(false), interactMode(INTERACT_NONE),printInfo(false) {
    nodes = new std::map<string,NodeGeometry* >();

}

PointCloud::~PointCloud() {
    // destroy tree
	if(pcinfo)
		delete pcinfo;
    delete nodes;
}

int PointCloud::initPointCloud() {

	// Option
	if(!option){
		cout << "Error: cannot load option " << endl;
		return -1;
	}
	if(master)
		Utils::printOption(option);

	// PC Info
	pcinfo = Utils::loadPCInfo(option->dataDir);
	if(!pcinfo) {
		cout << "Error: cannot load pc info" << endl;
		return -1;
	}
	if(master)
		Utils::printPCInfo(pcinfo);

	// Material
    if (!material)
        material = new Material(option);

	// root node
	string name = "r";
	root = new NodeGeometry(name);
	root->setInfo(pcinfo);
    if(root->loadHierachy(nodes)) {
		cout << "fail to load root hierachy" << endl;
		return -1;
	}
	if(root->loadData()) {
		cout << "fail to load root data " << endl;
		return -1;
	}

    //preDisplayListSize = 0;

	numLoaderThread = option->numReadThread;
	// reading threads
	if(nodeLoaderThreads.size() == 0) {
    	for(int i = 0; i < numLoaderThread; i++) {
    		NodeLoaderThread* t = new NodeLoaderThread(nodeQueue, option->maxLoadSize);
    		t->start();
    		nodeLoaderThreads.push_back(t);
	    }
	
    }
    if (!lrucache)
        lrucache = new LRUCache(option->maxNodeInMem);

    preloadUpToLevel(option->preloadToLevel);

	return 1;
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
		if(numloaded + node->getNumPoints() < option->visiblePointTarget)
    		canload = true;

    	if(!canload)
    		continue;

        node->loadHierachy(nodes);
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
    if (pauseUpdate)
        return 0;
	float V[6][4];
    Utils::getFrustum(V, MVP);
	    
    displayList.clear();
    numVisibleNodes = 0;
    numVisiblePoints = 0;

    unsigned int start_time = Utils::getTime();
    if (!root)
        return 0;
    root->loadHierachy(nodes);

    if (option->onlineUpdate) {
        //Utils::updatePCInfo(option->dataDir,root->getInfo());

        //root->checkForUpdate();
        root->Update();
    }


    priority_queue<NodeWeight> priority_queue;
    priority_queue.push(NodeWeight(root, 1));

    while(priority_queue.size() > 0){
    	NodeGeometry* node = priority_queue.top().node;
    	priority_queue.pop();
    	bool visible = false;

        if (option->onlineUpdate)
            node->Update();

    	if(Utils::testFrustum(V, node->getBBox()) >= 0 && numVisiblePoints + node->getNumPoints() < option->visiblePointTarget)
    		visible = true;
	    
	    if(!visible)
	    	continue; 

	    numVisibleNodes++;
		numVisiblePoints += node->getNumPoints();

        node->loadHierachy(nodes);

       // if (option->onlineUpdate) {
       //     node->checkForUpdate();
       // }


        // add to load queue
        /* debugging code:
        bool niq=node->inQueue();
        bool ncaq=node->canAddToQueue();
        bool nd=node->isDirty();
        bool nU=node->isUpdating();
        */
        if(!node->inQueue() && node->canAddToQueue() ) {
			node->setInQueue(true);
            //cout << "adding " << node->getName() << " to queue" << niq << ncaq << endl;
			nodeQueue.add(node);
		}
        else if (node->isDirty() && !node->isUpdating() ) {
            node->setInQueue(true);
            //cout << "adding " << node->getName() << " to queue because its dirty" << endl;
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

			float screenpixelradius = option->screenHeight * pr;
			if(screenpixelradius < option->minNodePixelSize)
				continue;

			priority_queue.push(NodeWeight(node->getChild(i), weight));
			//priority_queue.push(NodeWeight(node->getChild(i), 1.0/node->getChild(i)->getLevel()));
		}

    }

    return 0;
}

void PointCloud::unload() {
    cout << "unloading everything" << endl;
    lrucache->clear();
    //unload all the nodes
    nodes->erase(nodes->begin(),nodes->end());
    root = NULL;
    displayList.clear();
    _unload=false;
}

void PointCloud::resetRootHierarchy() {
    root->loadHierachy(nodes,true);
}

void PointCloud::flagNodeAsDirty(const std::string &nodename)
{
    NodeGeometry * node=NULL;
    if (nodes->find(nodename) != nodes->end())
    {
        node =(*nodes)[nodename];
        node->setDirty();
    } else {
        //we have to find the next hierarchy node and mark that as dirty
        bool foundHierarchyNode = false;
        string parentname = nodename;
        while (!foundHierarchyNode) {
             parentname = parentname.substr(0,parentname.size()-1);
             node =(*nodes)[parentname];
             if (node->canLoadHierarchy())
             {
                 foundHierarchyNode=true;
                 node->loadHierachy(nodes,false,true);
             }
        }
    }
}

void PointCloud::reload() {
    render = false;
    pauseUpdate=true;
    //empty loading queue
    if (0!=nodeQueue.size())
    {
        cout << "waiting for loading queue to empty" << endl;
        return;
    }
    //empty lru
    lrucache->clear();
    //unload all the nodes
    nodes->erase(nodes->begin(),nodes->end());
    displayList.clear();
    //redo init
    initPointCloud();
    needReloadShader = true;
    pauseUpdate=false;
    fullReload = false;
    render = true;
}

void PointCloud::debug() {
    Utils::printPCInfo(root->getInfo());
    cout << "numVisibleNodes: " << numVisibleNodes << " numVisiblePoints: " << numVisiblePoints <<
            " nodeQueue size: " << nodeQueue.size() << " lrucache size: " << lrucache->size() <<
         "total number of nodes: " << nodes->size() << endl;

    for(map<string, NodeGeometry*>::iterator it = nodes->begin(); it != nodes->end(); it++) {
        NodeGeometry* node=(*it).second;
        //cout << "Node: " << (*it).first << endl;
        node->printInfo();
    }
    printInfo = false;
}

void PointCloud::draw() {

	if(!material) {
		material = new Material(option);
		if(oglError) return;
	}

    if (_unload)
        unload();

    if (fullReload)
        reload();

	if(needReloadShader) {
		material->reloadShader(); 
		needReloadShader = false;
		if(oglError) return;
	}
	if(printInfo) {
        debug();
	}


#ifdef OMEGALIB_APP

    if (!render)
        return;

	glAlphaFunc(GL_GREATER, 0.1);
    glEnable(GL_ALPHA_TEST);
#endif
	for(list<NodeGeometry*>::iterator it = displayList.begin(); it != displayList.end(); it++) {
		NodeGeometry* node = *it;
		node->draw(material);
	}

	// draw interaction
	if(interactMode == INTERACT_NONE)
		return;
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	//draw ray line
	Vector3f spos = ray.getOrigin(); //- 1*ray.getDirection();
	Vector3f epos = ray.getOrigin() + 100*ray.getDirection();
	glLineWidth(4.0); 
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(spos[0], spos[1], spos[2]);
	glVertex3f(epos[0], epos[1], epos[2]);
	glEnd();

	glEnable(GL_PROGRAM_POINT_SIZE_EXT);
    glPointSize(40);
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_POINTS);
    for(int i=0; i < hitPoints.size(); i++) {
    	glVertex3f(hitPoints[i]->position[0], hitPoints[i]->position[1], hitPoints[i]->position[2]);
    }
    glEnd();
}

// interaction
void PointCloud::updateRay(const omega::Ray& r) {
	ray = r;
}

void PointCloud::findHitPoint() {
	if (interactMode == INTERACT_NONE)
		return;

	if (interactMode == INTERACT_POINT) {
		unsigned int start_time = Utils::getTime();
		hitPoints.clear();
		HitPoint* point = new HitPoint();

		for(list<NodeGeometry*>::iterator it = displayList.begin(); it != displayList.end(); it++) {
			NodeGeometry* node = *it;
			node->findHitPoint(ray, point);
		}

		if(point->distance != -1) {
			hitPoints.push_back(point);
			cout << "find time: " << Utils::getTime() - start_time << " size: " << hitPoints.size()
				<< " dis: " << hitPoints[0]->distance
			 	<< " pos: " << hitPoints[0]->position[0] << " " << hitPoints[0]->position[1] << " " 
			 	<< hitPoints[0]->position[2] << endl;
		}
		else {
			cout << "MISS" << endl;
		}
	}
}

}; //namespace gigapoint

