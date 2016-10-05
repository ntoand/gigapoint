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
};

#endif