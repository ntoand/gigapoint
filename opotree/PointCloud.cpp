#include "PointCloud.h"
#include "Utils.h"

#include <iostream>

using namespace std;

PointCloud::PointCloud(string datadir) {

	// Option
	option.visiblePointTarget = 100000;
	option.minNodePixelSize = 100;

	// PC Info
	if(PCLoader::loadPCInfo(datadir, pcinfo) != 0) {
		cout << "Error: cannot load pc info" << endl;
		return;
	}
	PCLoader::printPCInfo(pcinfo);

	// Shader + material
	list<string> attributes;
	list<string> uniforms;
	attributes.clear(); uniforms.clear();
	attributes.push_back("VertexPosition");
	uniforms.push_back("MVP");
	Shader* shader = new Shader("simple");
	shader->load("shaders/simple", attributes, uniforms);

	material = new Material(shader);

	// root node
	string name = "r";
	root = new NodeGeometry(name);
	if(root->loadBinData(pcinfo)) {
		cout << "fail to load node: " << name << endl;
		return;
	}
	root->parent = NULL;
	root->printInfo();

}

PointCloud::~PointCloud() {
	// desktroy tree

}

int PointCloud::updateVisibility(const float MVP[16]) {
	float V[6][4];
    Utils::getFrustum(V, MVP);

    priority_queue<NodeWeight> priority_queue;

    unsigned int point_count = 0;

    NodeWeight nw(root, 1);
    priority_queue.push(nw);
    displayList.clear();

    while(priority_queue.size() > 0){
    	NodeGeometry* node = priority_queue.top().node;
    	priority_queue.pop();
    	bool visible = false;
    	if(Utils::testFrustum(V, node->getBBox()) >= 0 && point_count + node->getNumPoints() < option.visiblePointTarget)
    		visible = true;
	       
	    if(visible) {
	    	displayList.push_back(node);
	    }
    }

	return 0;
}

void PointCloud::draw(const float MVP[16]) {

	Shader* shader = material->getShader();
	shader->bind();
    shader->transmitUniform("MVP", MVP);

	for(list<NodeGeometry*>::iterator it = displayList.begin(); it != displayList.end(); it++) {
		NodeGeometry* node = *it;
		node->initVBO(shader);
		node->draw();
	}
}