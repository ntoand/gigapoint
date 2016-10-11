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

		// other settings
		info.dataDir = data_dir;

		info.boundingBoxCentre[0] = (info.boundingBox[0] + info.boundingBox[3]) / 2;
		info.boundingBoxCentre[1] = (info.boundingBox[1] + info.boundingBox[4]) / 2;
		info.boundingBoxCentre[2] = (info.boundingBox[2] + info.boundingBox[5]) / 2;
		//for(int i=0; i < 3; i++)
		//	info.boundingBoxCentre[i] = info.boundingBox[i];
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

void PCLoader::addVectors(const float v1[3], const float v2[3], float v[3]) {
	for(int i=0; i < 3; i++)
		v[i] = v1[i] + v2[i];
}

void PCLoader::addVectors(const float v1[3], const float v2[3], const float v3[3], float v[3]) {
	for(int i=0; i < 3; i++)
		v[i] = v1[i] + v2[i] + v3[i];
}

int PCLoader::createChildAABB(const float pbbox[6], const int childIndex, float cbbox[6]) {
	float bmin[3];
	float bmax[3];
	
	float dHalfLength[3] = { (pbbox[3] - pbbox[0]) * 0.5,
							 (pbbox[4] - pbbox[1]) * 0.5,
							 (pbbox[5] - pbbox[2]) * 0.5 };
	float xHalfLength[3] = { dHalfLength[0], 0, 0 };
	float yHalfLength[3] = { 0, dHalfLength[1], 0 };
	float zHalfLength[3] = { 0, 0, dHalfLength[2] };

	float cmin[3] = { pbbox[0], pbbox[1], pbbox[2] };
	float cmax[3] = { pbbox[0] + dHalfLength[0], 
					  pbbox[1] + dHalfLength[1], 
					  pbbox[2] + dHalfLength[2] };

	switch (childIndex) {
		case 1:
			addVectors(cmin, zHalfLength, bmin);
			addVectors(cmax, zHalfLength, bmax);
			break;

		case 3:
			addVectors(cmin, zHalfLength, yHalfLength, bmin);
			addVectors(cmax, zHalfLength, yHalfLength, bmax);
			break;

		case 0:
			for(int i=0; i < 3; i++) {
				bmin[i] = cmin[i]; bmax[i] = cmax[i];
			}
			break;

		case 2:
			addVectors(cmin, yHalfLength, bmin);
			addVectors(cmax, yHalfLength, bmax);
			break;

		case 5:
			addVectors(cmin, zHalfLength, xHalfLength, bmin);
			addVectors(cmax, zHalfLength, xHalfLength, bmax);
			break;

		case 7:
			addVectors(cmin, dHalfLength, bmin);
			addVectors(cmax, dHalfLength, bmax);
			break;

		case 4:
			addVectors(cmin, xHalfLength, bmin);
			addVectors(cmax, xHalfLength, bmax);
			break;

		case 6:
			addVectors(cmin, xHalfLength, yHalfLength, bmin);
			addVectors(cmax, xHalfLength, yHalfLength, bmax);
			break;

		defaut:
			break;
	};

	cbbox[0] = bmin[0]; cbbox[1] = bmin[1]; cbbox[2] = bmin[2];
	cbbox[3] = bmax[0]; cbbox[4] = bmax[1]; cbbox[5] = bmax[2];

	return 0;
}
