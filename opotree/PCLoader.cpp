#include "PCLoader.h"
#include "cJSON.h"

#include <iostream>
#include <fstream>
#include <assert.h>

using namespace std;

int PCLoader::loadPCInfo(const string data_dir, PCInfo& info) {
	
	string filename = data_dir + "cloud.js";
	cout << "Load PC info from file: " << filename << endl;

	FILE *f;long len;char *data;
	f=fopen(filename.c_str(),"rb");
	if(f == NULL){
		std::cout << "Cannot find default.cfg file!!!" << std::endl;
		return -1;
	}
	fseek(f,0,SEEK_END);len=ftell(f);fseek(f,0,SEEK_SET);
	data= new char[len+1];fread(data,1,len,f);fclose(f);

	cJSON *json;

	json=cJSON_Parse(data);

	if (!json) {
		cout << "Error before:" << endl << cJSON_GetErrorPtr() << endl;
		return -1;
	}
	else {

		info.version = cJSON_GetObjectItem(json, "version")->valuestring;
		info.octreeDir = cJSON_GetObjectItem(json, "octreeDir")->valuestring;

		cJSON *bbox = cJSON_GetObjectItem(json, "boundingBox");
		assert(bbox);
		info.boundingBox[0] = cJSON_GetObjectItem(bbox, "lx")->valuedouble;
		info.boundingBox[1] = cJSON_GetObjectItem(bbox, "ly")->valuedouble;
		info.boundingBox[2] = cJSON_GetObjectItem(bbox, "lz")->valuedouble;
		info.boundingBox[3] = cJSON_GetObjectItem(bbox, "ux")->valuedouble;
		info.boundingBox[4] = cJSON_GetObjectItem(bbox, "uy")->valuedouble;
		info.boundingBox[5] = cJSON_GetObjectItem(bbox, "uz")->valuedouble;

		cJSON *tbbox = cJSON_GetObjectItem(json, "tightBoundingBox");
		assert(tbbox);
		info.tightBoundingBox[0] = cJSON_GetObjectItem(tbbox, "lx")->valuedouble;
		info.tightBoundingBox[1] = cJSON_GetObjectItem(tbbox, "ly")->valuedouble;
		info.tightBoundingBox[2] = cJSON_GetObjectItem(tbbox, "lz")->valuedouble;
		info.tightBoundingBox[3] = cJSON_GetObjectItem(tbbox, "ux")->valuedouble;
		info.tightBoundingBox[4] = cJSON_GetObjectItem(tbbox, "uy")->valuedouble;
		info.tightBoundingBox[5] = cJSON_GetObjectItem(tbbox, "uz")->valuedouble;

		cJSON *pointatt = cJSON_GetObjectItem(json, "pointAttributes");
		assert(pointatt);
		info.pointByteSize = 0;
		for (int i = 0; i < cJSON_GetArraySize(pointatt); i++) {
			string data_type = cJSON_GetArrayItem(pointatt, i)->valuestring;
			if(data_type == "POSITION_CARTESIAN") {
				info.pointAttributes.push_back(POSITION_CARTESIAN);
				info.pointByteSize += 3 * sizeof(float);
			}
			else if(data_type == "COLOR_PACKED") {
				info.pointAttributes.push_back(COLOR_PACKED);
				info.pointByteSize += 4 * sizeof(char);
			}
			else {
				cout << "Invalid data type" << endl;
				return -1;
			}
		}
		cout << endl;

		info.spacing = cJSON_GetObjectItem(json, "spacing")->valuedouble;
		info.scale = cJSON_GetObjectItem(json, "scale")->valuedouble;
		info.hierarchyStepSize = cJSON_GetObjectItem(json, "hierarchyStepSize")->valueint;

		info.dataDir = data_dir;
	}

	cJSON_Delete(json);
	delete [] data;

	return 0;
}

void PCLoader::printPCInfo(const PCInfo& info) {
	cout << "==== PC Info ====" << endl;
	cout << "Version: " << info.version << endl;
	cout << "dataDir: " << info.dataDir << endl;
	cout << "octreeDir: " << info.octreeDir << endl;
	cout << "boundingBox: ";
	for(int i=0; i < 6; i++)
		cout << info.boundingBox[i] << " ";
	cout << endl;

	cout << "tightBoundingBox: ";
	for(int i=0; i < 6; i++)
		cout << info.tightBoundingBox[i] << " ";
	cout << endl;

	cout << "pointAttributes: ";
	for(int i=0; i < info.pointAttributes.size(); i++)
		cout << info.pointAttributes[i] << " ";
	cout << endl;

	cout << "Spacing: " << info.spacing << endl;
	cout << "Scale: " << info.scale << endl;
	cout << "hierarchyStepSize: " << info.hierarchyStepSize << endl << endl;
}
