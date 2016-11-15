#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

#define PI 3.14159265f

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define DEG(r) (180.0f * (r) / PI)
#define RAD(d) (PI * (d) / 180.0f)

#define DOT3(v, w) ((v)[0] * (w)[0] + \
                    (v)[1] * (w)[1] + \
                    (v)[2] * (w)[2])


#define POSITION_CARTESIAN 0
#define COLOR_PACKED 1
#define INTENSITY 2
#define CLASSIFICATION 3

#define MATERIAL_RGB 0
#define MATERIAL_ELEVATION 1
#define MATERIAL_TREEDEPTH 2

#define SIZE_FIXED 0
#define SIZE_ADAPTIVE 1

#define QUALITY_SQUARE 0
#define QUALITY_CIRCLE 1

typedef struct Option_t {
	string dataDir;
	unsigned int visiblePointTarget;
	float minNodePixelSize;
	float screenHeight;
	bool moveToCentre;
	int material;
	float pointSize;
	int sizeType;
	int quality;
	int numReadThread;
	int preloadToLevel;
	int maxNodeInMem;
	float cameraSpeed;
	bool cameraUpdatePosOri;
	float cameraPosition[3];
	float cameraOrientation[4];
	float scaleXYZ[3];
} Option;

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

class Utils {

public:
	static unsigned int getTime();

	static int testPlane(const float V[4], const float b[6]);
	static int testFrustum(float V[6][4], const float b[6]);
	static void getFrustum(float V[6][4], const float X[16]);
	static char* getFileContent(std::string path);
	static float distance(const float v1[3], const float v2[3]);

	// PC loader
	static int loadOption(const string cfgfile, Option& option);
	static void printOption(const Option& option);
	static int loadPCInfo(const string data_dir, PCInfo* info);
	static void printPCInfo(const PCInfo* info);
	static void addVectors(const float v1[3], const float v2[3], float v[3]);
	static void addVectors(const float v1[3], const float v2[3], const float v3[3], float v[3]);
	static int createChildAABB(const float pbbox[6], const int childIndex, float cbbox[6]);
};

#endif