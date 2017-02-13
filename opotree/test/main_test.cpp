#include <iostream>
#include <bitset>
#include <fstream>

#include "PCLoader.h"

using namespace std;

int loadHierachy(string filename) {
	cout << "Load hierachy file: " << filename << endl;

	FILE *f;long len; char *data;
	f=fopen(filename.c_str(),"rb");
	if(f == NULL){
		std::cout << "Cannot find " << filename << "!!!" << std::endl;
		return -1;
	}
	fseek(f,0,SEEK_END);len=ftell(f);fseek(f,0,SEEK_SET);
	data= new char[len+1];fread(data,1,len,f);fclose(f);

	cout << "Lenght: " << len << endl;

	// root of subtree
	int offset = 0;
	unsigned char children = data[offset];
	unsigned int numpoints = (data[offset+4] << 24) | (data[offset+3] << 16) | (data[offset+2] << 8) | data[offset+1]; // little andian
	offset += 5;

	std::bitset<8> x(children);
	cout << "Root children: " << x << endl;
	cout << "Root numpoints: " << numpoints << endl;

	// other nodes
	

	return 0;
}

int loadDataFile(PCInfo info, bool movetocentre, string filename) {

	ifstream reader;
	reader.open (filename.c_str(), ifstream::in | ifstream::binary);
	if(reader == NULL) {
		cout << "cannot open file " << filename << endl;
		return 1;
	}

	vector<float> vertices;

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
				unsigned int ux, uy, uz;
				unsigned char* iBuffer = reinterpret_cast<unsigned char*>(buffer+offset);
				ux = (iBuffer[offset+3] << 24) | (iBuffer[offset+2] << 16) | (iBuffer[offset+1] << 8) | iBuffer[offset];
				offset += 4;
				uy = (iBuffer[offset+3] << 24) | (iBuffer[offset+2] << 16) | (iBuffer[offset+1] << 8) | iBuffer[offset];
				offset += 4;
				uz = (iBuffer[offset+3] << 24) | (iBuffer[offset+2] << 16) | (iBuffer[offset+1] << 8) | iBuffer[offset];
				offset += 4;
				float x = (ux * info.scale) + info.boundingBox[0];
				float y = (uy * info.scale) + info.boundingBox[1];
				float z = (uz * info.scale) + info.boundingBox[2];
				if (movetocentre) {
					x -= info.boundingBoxCentre[0];
					y -= info.boundingBoxCentre[1];
					z -= info.boundingBoxCentre[2];
				}	
				vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
				
				//offset += 3 * sizeof(float);
				//cout << "pos: " << x << " " << y << " " << z << endl;

			}else if(attribute == COLOR_PACKED){
				unsigned char* ucBuffer = reinterpret_cast<unsigned char*>(buffer+offset);
				float r = ucBuffer[0]/255.0;
				float g = ucBuffer[1]/255.0;
				float b = ucBuffer[2]/255.0;
				offset += 4 * sizeof(char);
				//cout << "colors: " << r << " " << g << " " << b << endl;

			}else {
				cout << "Error: Invalid attribute!" << endl;
			}
		}
		
		delete [] buffer;
	}

	float bbox[6];
	bbox[0] = bbox[3] = vertices[0];
	bbox[1] = bbox[4] = vertices[1];
	bbox[2] = bbox[5] = vertices[2];

	for(int i=3; i < vertices.size(); i+=3) {
		bbox[0] = min(bbox[0], vertices[i]); bbox[1] = min(bbox[1], vertices[i+1]); bbox[2] = min(bbox[2], vertices[i+2]);
		bbox[3] = max(bbox[3], vertices[i]); bbox[4] = max(bbox[4], vertices[i+1]); bbox[5] = max(bbox[5], vertices[i+2]);
	}

	cout << endl << filename << endl;
	cout << "Some points:" << endl;
	for(int i=0; i < 20; i++)
		cout << vertices[i*3] << " " << vertices[i*3+1] << " " << vertices[i*3+2] << endl;
	cout << "BBox: ";
	for(int i=0; i < 6; i++)
		cout << bbox[i] << " ";
	cout << endl;

	reader.close();
	return 0;
}

int main (int argc, char *argv[]) {

	cout << "Done!" << endl;

	//string datadir = "ripple/";
	string datadir = "P100/";

	PCInfo pcinfo;
	if(PCLoader::loadPCInfo(datadir, pcinfo) != 0) {
		cout << "Error: cannot load pc info" << endl;
		return 1;
	}
	PCLoader::printPCInfo(pcinfo);

	string hrc_filename = datadir + "data/r/r.hrc";
	loadHierachy(hrc_filename);

	vector<string> filenames;
	filenames.push_back("p100/data/r/r.bin");
	filenames.push_back("p100/data/r/r0.bin");
	filenames.push_back("p100/data/r/r1.bin");
	filenames.push_back("p100/data/r/r2.bin");
	filenames.push_back("p100/data/r/r3.bin");
	filenames.push_back("p100/data/r/r4.bin");
	filenames.push_back("p100/data/r/r5.bin");
	filenames.push_back("p100/data/r/r6.bin");
	//filenames.push_back("p100/data/r/r7.bin");

	for(int i=0; i < filenames.size(); i++)
		loadDataFile(pcinfo, true, filenames[i]);

	return 0;
}