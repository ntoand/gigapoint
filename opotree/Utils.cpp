#include "Utils.h"
#include <math.h>

using namespace std;

int Utils::testPlane( const float V[4], const float b[6])
{
    const float k00 = b[0] * V[0];
    const float k11 = b[1] * V[1];
    const float k22 = b[2] * V[2];
    const float k30 = b[3] * V[0];
    const float k41 = b[4] * V[1];
    const float k52 = b[5] * V[2];

    int c = 0;

    // Test all 8 points of the bounding box against this plane.

    if (k00 + k11 + k22 + V[3] > 0) c++;
    if (k00 + k11 + k52 + V[3] > 0) c++;
    if (k00 + k41 + k22 + V[3] > 0) c++;
    if (k00 + k41 + k52 + V[3] > 0) c++;
    if (k30 + k11 + k22 + V[3] > 0) c++;
    if (k30 + k11 + k52 + V[3] > 0) c++;
    if (k30 + k41 + k22 + V[3] > 0) c++;
    if (k30 + k41 + k52 + V[3] > 0) c++;

    // Return the number of points in front of the plane.

    return c;
}

int Utils::testFrustum(float V[6][4], const float b[6])
{
    int c0, c1, c2, c3, c4, c5;

    // If the bounding box is entirely behind any of the planes, return -1.

    if ((c0 = testPlane(V[0], b)) == 0) return -1;
    if ((c1 = testPlane(V[1], b)) == 0) return -1;
    if ((c2 = testPlane(V[2], b)) == 0) return -1;
    if ((c3 = testPlane(V[3], b)) == 0) return -1;
    if ((c4 = testPlane(V[4], b)) == 0) return -1;
    if ((c5 = testPlane(V[5], b)) == 0) return -1;

    // If the box is entirely in view, return +1.  If split, return 0.

    return (c0 + c1 + c2 + c3 + c4 + c5 == 48) ? 1 : 0;
}

void Utils::getFrustum(float V[6][4], const float X[16])
{
    int i;

    /* Left plane. */

    V[0][0] = X[3]  + X[0];
    V[0][1] = X[7]  + X[4];
    V[0][2] = X[11] + X[8];
    V[0][3] = X[15] + X[12];

    /* Right plane. */

    V[1][0] = X[3]  - X[0];
    V[1][1] = X[7]  - X[4];
    V[1][2] = X[11] - X[8];
    V[1][3] = X[15] - X[12];

    /* Bottom plane. */

    V[2][0] = X[3]  + X[1];
    V[2][1] = X[7]  + X[5];
    V[2][2] = X[11] + X[9];
    V[2][3] = X[15] + X[13];

    /* Top plane. */

    V[3][0] = X[3]  - X[1];
    V[3][1] = X[7]  - X[5];
    V[3][2] = X[11] - X[9];
    V[3][3] = X[15] - X[13];

    /* Near plane. */

    V[4][0] = X[3]  + X[2];
    V[4][1] = X[7]  + X[6];
    V[4][2] = X[11] + X[10];
    V[4][3] = X[15] + X[14];

    /* Far plane. */

    V[5][0] = X[3]  - X[2];
    V[5][1] = X[7]  - X[6];
    V[5][2] = X[11] - X[10];
    V[5][3] = X[15] - X[14];

    /* Normalize all plane vectors. */

    for (i = 0; i < 6; ++i)
    {
        float k = (float) sqrt(DOT3(V[i], V[i]));

        V[i][0] /= k;
        V[i][1] /= k;
        V[i][2] /= k;
        V[i][3] /= k;
    }
}

char* Utils::getFileContent(string path) {
    FILE* fp;
    char* content = NULL;
    long length;

    fp = fopen( path.c_str(), "rb" );
    if (fp)
    {
        fseek( fp, 0, SEEK_END );
        length = ftell( fp );
        fseek( fp, 0, SEEK_SET );
        content = new char [length+1];
        fread( content, sizeof( char ), length, fp );
        fclose( fp );
        content[length] = '\0';
    }

    return content;
}