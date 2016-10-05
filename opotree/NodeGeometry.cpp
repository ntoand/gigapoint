#include "Utils.h"
#include "NodeGeometry.h"

#include <iostream>
#include <fstream>
#include <assert.h>
#include <float.h>

using namespace std;
using namespace omega;

NodeGeometry::NodeGeometry(string _name): numpoints(0), level(-1), parent(NULL),
										  loaded(false), initvbo(false) {
	name = _name;
	bbox[0] = bbox[1] = bbox[2] = FLT_MAX;
	bbox[3] = bbox[4] = bbox[5] = FLT_MIN;
	for(int i=0; i < 8; i++)
		children[i] = NULL;
}

NodeGeometry::~NodeGeometry() {

}

void NodeGeometry::addPoint(float x, float y, float z) {
	vertices.push_back(x); 
	vertices.push_back(y);
	vertices.push_back(z);
	
	numpoints++;

	bbox[0] = min(bbox[0], x); bbox[1] = min(bbox[1], y); bbox[2] = min(bbox[2], z);
	bbox[3] = max(bbox[3], x); bbox[4] = max(bbox[4], y); bbox[5] = max(bbox[5], z);
}

void NodeGeometry::addColor(float r, float g, float b) {
	colors.push_back(r); 
	colors.push_back(g);
	colors.push_back(b);
}

int NodeGeometry::loadBinData(const PCInfo info, bool plusbbmin) {
	string filename = info.dataDir + info.octreeDir + "/r/" + name + ".bin";
	cout << "Load file: " << filename << endl;
	ifstream reader;
	reader.open (filename.c_str(), ifstream::in | ifstream::binary);
	
	while(reader.good()) {
		char* buffer = new char[info.pointByteSize];
		reader.read(buffer, info.pointByteSize);

		if(!reader.good()){
            delete [] buffer;
			break;
		}

		int offset = 0;
		for(int i = 0; i < info.pointAttributes.size(); i++){
			int attribute = info.pointAttributes[i];
			
			if(attribute == POSITION_CARTESIAN){
				int* iBuffer = reinterpret_cast<int*>(buffer+offset);
				float x, y, z;
				if (plusbbmin) {
					x = (iBuffer[0] * info.scale) + info.boundingBox[0];
					y = (iBuffer[1] * info.scale) + info.boundingBox[1];
					z = (iBuffer[2] * info.scale) + info.boundingBox[2];
				} else {
					x = iBuffer[0] * info.scale;
					y = iBuffer[1] * info.scale;
					z = iBuffer[2] * info.scale;
				}
				
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
		
		delete [] buffer;
	}

	reader.close();

	loaded = true;

	return 0;
}

void NodeGeometry::printInfo() {
	cout << endl << "Node: " << name << endl;
	cout << "# points: " << numpoints << endl;
	cout << "bbox: ";
	for(int i=0; i < 6; i++)
		cout << bbox[i] << " ";
	cout << endl;
	cout << "first 10 points: " << endl;
	for(int i=0; i < min(numpoints, 10); i++) {
		cout << vertices[i*3+0] << " " << vertices[i*3+1] << " " << vertices[i*3+2] << "   ";
		if(colors.size() > 0) 
			cout << colors[i*3+0] << " " << colors[i*3+1] << " " << colors[i*3+2];
		cout << endl;
	}
}

int NodeGeometry::initVBO(Shader* sh) {

	if(!loaded || initvbo)
		return 1;

	shader = sh;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size(), &vertices[0], GL_STATIC_DRAW);

	unsigned int attribute_vertex_pos = shader->attribute("VertexPosition");
    glEnableVertexAttribArray(attribute_vertex_pos);  // Vertex position
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(
        attribute_vertex_pos, // attribute
        3,                 // number of elements per vertex, here (x,y,z)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
    );

    initvbo = true;

    return 0;
}

void NodeGeometry::draw() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glDrawArrays(GL_POINTS, 0, numpoints);
}