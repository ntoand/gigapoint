#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>

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

class Utils {

public:
	static int testPlane(const float V[4], const float b[6]);
	static int testFrustum(float V[6][4], const float b[6]);
	static void getFrustum(float V[6][4], float X[16]);
	static char* getFileContent(std::string path);

};

#endif