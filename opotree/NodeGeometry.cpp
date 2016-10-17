#include "Utils.h"
#include "NodeGeometry.h"

#include <iostream>
#include <fstream>
#include <assert.h>
#include <float.h>
#include <bitset>
#include <map>

using namespace std;
using namespace omega;

NodeGeometry::NodeGeometry(string _name): numpoints(0), level(-1), parent(NULL), index(-1),
										  loaded(false), initvbo(false), haschildren(false),
										  hierachyloaded(false), inqueue(false),
										  vertexbuffer(-1), colorbuffer(-1), loading(false) {
	name = _name;
	//tightbbox[0] = tightbbox[1] = tightbbox[2] = FLT_MAX;
	//tightbbox[3] = tightbbox[4] = tightbbox[5] = FLT_MIN;
	for(int i=0; i < 8; i++)
		children[i] = NULL;
	level = name.length() - 1;
	for(int i=0; i < 6; i++)
		bbox[i] = -1;
}

NodeGeometry::~NodeGeometry() {

}

void NodeGeometry::addPoint(float x, float y, float z) {
	vertices.push_back(x); 
	vertices.push_back(y);
	vertices.push_back(z);
	
	//numpoints++;
	//tightbbox[0] = min(tightbbox[0], x); tightbbox[1] = min(tightbbox[1], y); tightbbox[2] = min(tightbbox[2], z);
	//tightbbox[3] = max(tightbbox[3], x); tightbbox[4] = max(tightbbox[4], y); tightbbox[5] = max(tightbbox[5], z);
}

void NodeGeometry::addColor(float r, float g, float b) {
	colors.push_back(r); 
	colors.push_back(g);
	colors.push_back(b);
}

void NodeGeometry::setBBox(const float* bb) {
	for(int i=0; i < 6; i++)
		bbox[i] = bb[i];
	spherecentre[0] = (bbox[0] + bbox[3])*0.5;
	spherecentre[1] = (bbox[1] + bbox[4])*0.5;
	spherecentre[2] = (bbox[2] + bbox[5])*0.5;
	float bmin[3] = { bbox[0], bbox[1], bbox[2] };
	sphereradius = Utils::distance(spherecentre, bmin);
}

string NodeGeometry::getHierarchyPath() {
	assert(info);
	string path = "r/";
	int numparts = level / info->hierarchyStepSize;
	for(int i=0; i < numparts; i++) {
		path += name.substr(1+i*info->hierarchyStepSize, info->hierarchyStepSize) + "/";
	}
	
	return path;
}

int NodeGeometry::loadHierachy(bool movetocentre) {

	if(level % info->hierarchyStepSize != 0)
		return 0;

	if(hierachyloaded)
		return 0;

	assert(info);

	if(level == 0) { // root
		if(movetocentre) {
			float b[6];
			for(int i=0; i < 6; i++)
				b[i] = info->boundingBox[i] - info->boundingBoxCentre[i%3];
			setBBox(b);
		} else {
			setBBox(info->boundingBox);
		}
	} 

	string hrc_filename = info->dataDir + info->octreeDir + "/" + getHierarchyPath() + name + ".hrc";
	//cout << "Load hierachy file: " << hrc_filename << endl;
	list<HRC_Item> stack;
	list<HRC_Item> decoded;

	FILE *f;long len; unsigned char *data;
	f=fopen(hrc_filename.c_str(),"rb");
	if(f == NULL){
		std::cout << "Cannot find " << hrc_filename << "!!!" << std::endl;
		return -1;
	}
	fseek(f,0,SEEK_END);len=ftell(f);fseek(f,0,SEEK_SET);
	data= new unsigned char[len+1];fread(data,1,len,f);fclose(f);

	// root of subtree
	int offset = 0;
	unsigned char children = data[offset];
	numpoints = (data[offset+4] << 24) | (data[offset+3] << 16) | (data[offset+2] << 8) | data[offset+1]; // little andian
	offset += 5;

	std::bitset<8> x(children);
	//cout << "Root children: " << x << endl;
	//cout << "Root numpoints: " << numpoints << endl;

	stack.push_back(HRC_Item(name, children, numpoints));

	while(stack.size() > 0){

		HRC_Item snode = stack.front();
		stack.pop_front();

		unsigned char mask = 1;
		for (int i=0; i < 8; i++) {
			if( (int)(snode.children & mask) != 0) {
				stringstream ss;
				ss << i;
				string childname = snode.name + ss.str();
				unsigned char child_children = data[offset];
				unsigned int child_numpoints = (data[offset+4] << 24) | (data[offset+3] << 16) | (data[offset+2] << 8) | data[offset+1];
				
				stack.push_back(HRC_Item(childname, child_children, child_numpoints));
				decoded.push_back(HRC_Item(childname, child_children, child_numpoints));

				offset += 5;
			}
			mask = mask * 2;
		}

		if(offset == len)
			break;
	}

	map<string, NodeGeometry*> nodes;
	nodes[name] = this;

	for(list<HRC_Item>::iterator it = decoded.begin(); it != decoded.end(); it++) {
		HRC_Item item = *it;
		std::bitset<8> children_bit(item.children);
		//cout << "Node: " << item.name << " children: " << children_bit << " numpoints: " << item.numpoints << endl;

		string str_ind = item.name.substr(item.name.length()-1, 1);
		//cout << "index str: " << str_ind << endl;

		string parentname = item.name.substr(0, item.name.length()-1);
		NodeGeometry* pnode = nodes[parentname];
		assert(pnode);

		NodeGeometry* cnode = new NodeGeometry(item.name);
		assert(cnode);
		int cindex = atoi(str_ind.c_str());
		cnode->setLevel(item.name.length()-1);
		cnode->setIndex(cindex);
		cnode->setNumPoints(item.numpoints);
		cnode->setHasChildren(item.children > 0);
		float cbbox[6];
		Utils::createChildAABB(pnode->getBBox(), cindex, cbbox);
		cnode->setBBox(cbbox);
		cnode->setInfo(pnode->getInfo());
		//cnode->printInfo();
	
		pnode->addChild(cnode);
		nodes[item.name] = cnode;
	}

	hierachyloaded = true;

	return 0;
}


int NodeGeometry::loadData() {

	if(loaded)
		return 0;
	
	assert(info);

	loading = true;

	string filename = info->dataDir + info->octreeDir + "/" + getHierarchyPath() + name + ".bin";
	//cout << "Load file: " << filename << endl;
	datafile = filename;
	ifstream reader;
	reader.open (filename.c_str(), ifstream::in | ifstream::binary);
	
	while(reader.good()) {
		char* buffer = new char[info->pointByteSize];
		reader.read(buffer, info->pointByteSize);

		if(!reader.good()){
			if(buffer)
            	delete [] buffer;
			break;
		}

		int offset = 0;
		for(int i = 0; i < info->pointAttributes.size(); i++){
			int attribute = info->pointAttributes[i];
			
			if(attribute == POSITION_CARTESIAN){
				int* iBuffer = reinterpret_cast<int*>(buffer+offset);
				float x, y, z;
				x = (iBuffer[0] * info->scale) + bbox[0];
				y = (iBuffer[1] * info->scale) + bbox[1];
				z = (iBuffer[2] * info->scale) + bbox[2];
			
				offset += 3 * sizeof(float);
				addPoint(x, y, z);
				//cout << "pos: " << x << " " << y << " " << z << endl;

			}else if(attribute == COLOR_PACKED){
				unsigned char* ucBuffer = reinterpret_cast<unsigned char*>(buffer+offset);
				float r = ucBuffer[0]/255.0;
				float g = ucBuffer[1]/255.0;
				float b = ucBuffer[2]/255.0;
				offset += 4 * sizeof(char);
				addColor(r, g, b);
				//cout << "colors: " << r << " " << g << " " << b << endl;

			}else {
				cout << "Error: Invalid attribute!" << endl;
			}
		}
		
		if(buffer)
			delete [] buffer;
	}

	reader.close();

	loading = false;
	loaded = true;

	return 0;
}

void NodeGeometry::printInfo() {
	cout << endl << "Node: " << name << " level: " << level << " index: " << index << endl;
	cout << "# points: " << numpoints << endl;
	cout << "data file: " << datafile << endl;
	cout << "bbox: ";
	for(int i=0; i < 6; i++)
		cout << bbox[i] << " ";
	cout << endl;

	/*
	cout << "tight bbox: ";
	for(int i=0; i < 6; i++)
		cout << tightbbox[i] << " ";
	cout << endl;
	*/

	if(!loaded)
		return;

	cout << "first 5 points: " << endl;
	for(int i=0; i < min(numpoints, 5); i++) {
		cout << vertices[i*3+0] << " " << vertices[i*3+1] << " " << vertices[i*3+2] << "   ";
		if(colors.size() > 0) 
			cout << colors[i*3+0] << " " << colors[i*3+1] << " " << colors[i*3+2];
		cout << endl;
	}

	if(initvbo)
		cout << "vertexbuffer: " << vertexbuffer << " colorbuffer: " << colorbuffer << endl;
}

int NodeGeometry::initVBO() {

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*4, &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size()*4, &colors[0], GL_STATIC_DRAW);

    initvbo = true;

    return 0;
}

void NodeGeometry::draw(Material* material) {
	if(loading || !loaded)
		return;

	if(!initvbo)
		initVBO();

	Shader* shader = material->getShader();
	material->bind();

    unsigned int attribute_vertex_pos = shader->attribute("VertexPosition");
    glEnableVertexAttribArray(attribute_vertex_pos);  // Vertex position
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        attribute_vertex_pos, // attribute
        3,                 // number of elements per vertex, here (x,y,z)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
    );

    unsigned int attribute_color_pos = shader->attribute("VertexColor");
    glEnableVertexAttribArray(attribute_color_pos);  // Vertex position
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
        attribute_color_pos, // attribute
        3,                 // number of elements per vertex, here (r, g, b)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
    );

    shader->transmitUniform("uScreenHeight", (float)material->getScreenHeight());
    shader->transmitUniform("uSpacing", (float)info->spacing);
    shader->transmitUniform("uPointSize", (float)material->getPointSize());
    shader->transmitUniform("uMinPointSize", 2.0f);
    shader->transmitUniform("uMaxPointSize", 40.0f);
    //shader->transmitUniform("uFOV", 0.785); // 45 degree

	glDrawArrays(GL_POINTS, 0, vertices.size()/3);
}

void NodeGeometry::freeData() {
	//cout << "Free data for node: " << name << endl;
	if(initvbo) {
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteBuffers(1, &colorbuffer);
		initvbo = false;
	}
	if(loaded) {
		vertices.clear();
		colors.clear();
		loaded = false;
	}
}