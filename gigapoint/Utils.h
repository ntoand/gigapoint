#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <vector>
#include <bitset>

using std::string;
using std::vector;

namespace gigapoint {


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

#define DIST3(v, w) (sqrt( \
                    ((v)[0]-(w)[0]) * ((v)[0]-(w)[0]) + \
                    ((v)[1]-(w)[1]) * ((v)[1]-(w)[1]) + \
                    ((v)[2]-(w)[2]) * ((v)[2]-(w)[2]) \
                    ))


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


// interaction
#define INTERACT_NONE	-1
#define INTERACT_POINT 	0
#define INTERACT_MULTIFRACTURE	1

#define MIN_TREE_DEPTH 6

#define FILTER_NONE 0
#define FILTER_EDL 1


typedef struct Option_t {
	string dataDir;
	string shaderDir;
	unsigned int visiblePointTarget;
	float minNodePixelSize;
	int material;
	float elevationRange[2];	//min, max in [0, 1]
	float pointScale[3];
	float pointSizeRange[2];
	int sizeType;
	int quality;
	int numReadThread;
    bool onlineUpdate;
	int preloadToLevel;
	int maxNodeInMem;
	int maxLoadSize;
	float cameraSpeed;
	bool cameraUpdatePosOri;
	float cameraPosition[3];
	float cameraOrientation[4];
	int filter;
	float filterEdl[2];			//[strength radius]

} Option;

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

class NodeGeometry;
#include <functional>

typedef struct PointIndex_ {
    //int treePath[MIN_TREE_DEPTH];
    int index;
    NodeGeometry *node;
    PointIndex_(NodeGeometry *_node, int _index) {
        index = _index;
        node=_node;
    }
    PointIndex_(){};
    bool operator> (const PointIndex_ &pi) {
        if (this->node==pi.node)
            return this->index>pi.index;
        return this->node>pi.node;
    }
    bool operator< (const PointIndex_ &pi) const {
        if (this->node==pi.node)
            return this->index<pi.index;
        return this->node<pi.node;
    }
    bool operator!= (const PointIndex_ &pi) {
        return pi.node!=this->node || pi.index != this->index;
    }
    bool operator== (const PointIndex_ &pi) {
        return pi.node==this->node && pi.index == this->index;
    }
    /*
    int ID()
    {
        if (id!=-1)
            return id;
        std::string str = node->getName()+std::to_string(index);
        std::hash<std::string> hasher;
        auto hashed = hasher(str); //returns std::size_t
        return id;
    }
    */
} PointIndex;

typedef struct Point_{
    unsigned char color[3];
    float position[3];
    PointIndex_ index;
    Point_(NodeGeometry *_node, int _index) {
        index=PointIndex(_node,_index);
    }
    Point_(const PointIndex &_index){index=_index;}
    Point_(){};
    bool operator== (const Point_ &p) {
       return ( this->index == p.index );
    }

    /*int ID()
    {
        return index.ID();
    }*/
} Point;

typedef struct HitPoint_ {
	float distance;
	float position[3];
    NodeGeometry *node;
    int index;
	HitPoint_() {
		distance = -1;
	}
} HitPoint;


// Utils class
class Utils {

public:
	static unsigned int getTime();
    static bool inCircle(const float segStart[3], const float segEnd[3], const float query[3]);
	static int testPlane(const float V[4], const float b[6]);
	static int testFrustum(float V[6][4], const float b[6]);
	static void getFrustum(float V[6][4], const float X[16]);
	static char* getFileContent(std::string path);
	static float distance(const float v1[3], const float v2[3]);

	// PC loader
	static Option* loadOption(const string cfgfile);
	static void printOption(const Option* option);
    static PCInfo* loadPCInfo(const string data_dir);
    static bool updatePCInfo(const string data_dir, PCInfo* pcinfo);
	static void printPCInfo(const PCInfo* info);
	static void addVectors(const float v1[3], const float v2[3], float v[3]);
	static void addVectors(const float v1[3], const float v2[3], const float v3[3], float v[3]);
	static int createChildAABB(const float pbbox[6], const int childIndex, float cbbox[6]);

};

}; //namespace gigapoint

#endif
