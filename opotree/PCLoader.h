#ifndef _PCLOADER_H_
#define _PCLOADER_H_

#include <string>
#include <vector>

using namespace std;

#define POSITION_CARTESIAN 0
#define COLOR_PACKED 1

typedef struct PCInfo_t {
	string version;
	string dataDir;
	string octreeDir;
	float boundingBox[6];
	float boundingBoxCentre[3];
	float tightBoundingBox[6];
	vector<int> pointAttributes;
	float spacing;
	float scale;
	int hierarchyStepSize;
	int pointByteSize;
} PCInfo;

class PCLoader {
private:

public:
	static int loadPCInfo(const string data_dir, PCInfo& info);
	static void printPCInfo(const PCInfo& info);

	static void addVectors(const float v1[3], const float v2[3], float v[3]);
	static void addVectors(const float v1[3], const float v2[3], const float v3[3], float v[3]);
	static int createChildAABB(const float pbbox[6], const int childIndex, float cbbox[6]);
};

#endif