#include "PointCloud.h"
#include "Utils.h"

#include <iostream>

using namespace std;

PointCloud::PointCloud(string cfgfile) {

	// Option
	if(Utils::loadOption(cfgfile, option) != 0){
		cout << "Error: cannot load option " << endl;
		return;
	}
	Utils::printOption(option);

	// PC Info
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
	uniforms.push_back("MVP");
	Shader* shader = new Shader("simple");
	shader->load("shaders/simple", attributes, uniforms);

	material = new Material(shader);

	// root node
	string name = "r";
	root = new NodeGeometry(name);
	if(root->loadData(pcinfo)) {
		cout << "fail to load node: " << name << endl;
		return;
	}

	preDisplayListSize = 0;
}

PointCloud::~PointCloud() {
	// desktroy tree

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

		node->loadData(pcinfo);
		displayList.push_back(node);
		
		// add children to priority_queue
		for(int i=0; i < 8; i++) {
			if(node->getChild(i) == NULL)
				continue;
			//calculte weight
			float* centre = node->getSphereCentre();
			float radius = node->getSphereRadius();
			float distance = Utils::distance(centre, campos);
			float fov = 0.436332;
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
    }
*/
    return 0;
}

void PointCloud::draw(const float MVP[16]) {

	Shader* shader = material->getShader();
	shader->bind();
    shader->transmitUniform("MVP", MVP);

	for(list<NodeGeometry*>::iterator it = displayList.begin(); it != displayList.end(); it++) {
		NodeGeometry* node = *it;
		node->draw(shader);
	}
}
