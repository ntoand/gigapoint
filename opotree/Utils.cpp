#include "Utils.h"
#include "cJSON.h"

#include <iostream>
#include <fstream>
#include <assert.h>
#include <math.h>
#include <stdio.h>

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

float Utils::distance(const float v1[3], const float v2[3]) {
    return sqrt( (v1[0]-v2[0])*(v1[0]-v2[0]) + (v1[1]-v2[1])*(v1[1]-v2[1]) + (v1[2]-v2[2])*(v1[2]-v2[2]) );
}

// Option
int Utils::loadOption(const string filename, Option& option) {
    cout << "Load option from file: " << filename << endl;

    FILE *f;long len;char *data;
    f=fopen(filename.c_str(),"rb");
    if(f == NULL){
        std::cout << "Cannot find " << filename << std::endl;
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
        option.dataDir = cJSON_GetObjectItem(json, "dataDir")->valuestring;
        option.dataDir.append("/");
        option.visiblePointTarget = cJSON_GetObjectItem(json, "visiblePointTarget")->valuedouble;
        option.minNodePixelSize = cJSON_GetObjectItem(json, "minNodePixelSize")->valuedouble;
        option.screenHeight = cJSON_GetObjectItem(json, "screenHeight")->valuedouble;
        option.moveToCentre = cJSON_GetObjectItem(json, "moveToCentre")->valueint > 0;

        string tmp = cJSON_GetObjectItem(json, "material")->valuestring;
        if (tmp.compare("rgb") == 0)
            option.material = MATERIAL_RGB;
        else if (tmp.compare("elevation") == 0)
            option.material = MATERIAL_ELEVATION;
        else if (tmp.compare("treedepth"))
            option.material = MATERIAL_TREEDEPTH;
        else
            option.material = MATERIAL_RGB;

        option.pointSize = cJSON_GetObjectItem(json, "pointSize")->valuedouble;

        tmp = cJSON_GetObjectItem(json, "sizeType")->valuestring;
        if (tmp.compare("fixed") == 0)
            option.sizeType = SIZE_FIXED;
        else if (tmp.compare("adaptive") == 0)
            option.sizeType = SIZE_ADAPTIVE;
        else
            option.sizeType = SIZE_FIXED;

        tmp = cJSON_GetObjectItem(json, "quality")->valuestring;
        if (tmp.compare("square") == 0)
            option.quality = QUALITY_SQUARE;
        else if (tmp.compare("adaptive") == 0)
            option.quality = QUALITY_CIRCLE;
        else
            option.quality = QUALITY_SQUARE;

        option.numReadThread = cJSON_GetObjectItem(json, "numReadThread")->valueint;
        option.maxNodeInMem = cJSON_GetObjectItem(json, "maxNodeInMem")->valueint;  
        option.cameraSpeed = cJSON_GetObjectItem(json, "cameraSpeed")->valueint;

        option.cameraUpdatePosOri = cJSON_GetObjectItem(json, "cameraUpdatePosOri")->valueint > 0;
        if(option.cameraUpdatePosOri) {
            cJSON* campos = cJSON_GetObjectItem(json, "cameraPosition");
            option.cameraPosition[0] = cJSON_GetArrayItem(campos, 0)->valuedouble;
            option.cameraPosition[1] = cJSON_GetArrayItem(campos, 1)->valuedouble;
            option.cameraPosition[2] = cJSON_GetArrayItem(campos, 2)->valuedouble;

            cJSON* camori = cJSON_GetObjectItem(json, "cameraOrientation");
            option.cameraOrientation[0] = cJSON_GetArrayItem(camori, 0)->valuedouble;
            option.cameraOrientation[1] = cJSON_GetArrayItem(camori, 1)->valuedouble;
            option.cameraOrientation[2] = cJSON_GetArrayItem(camori, 2)->valuedouble;
            option.cameraOrientation[3] = cJSON_GetArrayItem(camori, 3)->valuedouble;
        }
    }

    cJSON_Delete(json);
    delete [] data;
    return 0;
}

void Utils::printOption(const Option& option) {
    cout << "==== OPTION ====" << endl;
    cout << "data dir: " << option.dataDir << endl;
    cout << "visiblePointTarget: " << option.visiblePointTarget << endl;
    cout << "minNodePixelSize: " << option.minNodePixelSize << endl;
    cout << "screenHeight: " << option.screenHeight << endl;
    cout << "moveToCentre: " << option.moveToCentre << endl;
    cout << "material: " << option.material << endl;
    cout << "pointSize: " << option.pointSize << endl;
    cout << "sizeType: " << option.sizeType << endl;
    cout << "quality: " << option.quality << endl;
    cout << "cameraSpeed: " << option.cameraSpeed << endl;
    cout << "numReadThread: " << option.numReadThread << endl;
    cout << "maxNodeInMem: " << option.maxNodeInMem << endl;
    cout << "cameraUpdatePosOri" << option.cameraUpdatePosOri << endl;
    if(option.cameraUpdatePosOri) {
        cout << "cameraPosition: ";
        for(int i=0; i < 3; i++)
            cout << option.cameraPosition[i] << " ";
        cout << endl;
        cout << "cameraOrientation: ";
        for(int i=0; i < 4; i++)
            cout << option.cameraOrientation[i] << " ";
        cout << endl;
    }
}

// PC Loader
int Utils::loadPCInfo(const string data_dir, PCInfo* info) {

    assert(info);
    
    string filename = data_dir + "cloud.js";
    cout << "Load PC info from file: " << filename << endl;

    FILE *f;long len;char *data;
    f=fopen(filename.c_str(),"rb");
    if(f == NULL){
        std::cout << "Cannot find " << filename << std::endl;
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

        info->version = cJSON_GetObjectItem(json, "version")->valuestring;
        info->octreeDir = cJSON_GetObjectItem(json, "octreeDir")->valuestring;

        cJSON *bbox = cJSON_GetObjectItem(json, "boundingBox");
        assert(bbox);
        info->boundingBox[0] = cJSON_GetObjectItem(bbox, "lx")->valuedouble;
        info->boundingBox[1] = cJSON_GetObjectItem(bbox, "ly")->valuedouble;
        info->boundingBox[2] = cJSON_GetObjectItem(bbox, "lz")->valuedouble;
        info->boundingBox[3] = cJSON_GetObjectItem(bbox, "ux")->valuedouble;
        info->boundingBox[4] = cJSON_GetObjectItem(bbox, "uy")->valuedouble;
        info->boundingBox[5] = cJSON_GetObjectItem(bbox, "uz")->valuedouble;

        cJSON *tbbox = cJSON_GetObjectItem(json, "tightBoundingBox");
        assert(tbbox);
        info->tightBoundingBox[0] = cJSON_GetObjectItem(tbbox, "lx")->valuedouble;
        info->tightBoundingBox[1] = cJSON_GetObjectItem(tbbox, "ly")->valuedouble;
        info->tightBoundingBox[2] = cJSON_GetObjectItem(tbbox, "lz")->valuedouble;
        info->tightBoundingBox[3] = cJSON_GetObjectItem(tbbox, "ux")->valuedouble;
        info->tightBoundingBox[4] = cJSON_GetObjectItem(tbbox, "uy")->valuedouble;
        info->tightBoundingBox[5] = cJSON_GetObjectItem(tbbox, "uz")->valuedouble;

        cJSON *pointatt = cJSON_GetObjectItem(json, "pointAttributes");
        assert(pointatt);
        info->pointByteSize = 0;
        for (int i = 0; i < cJSON_GetArraySize(pointatt); i++) {
            string data_type = cJSON_GetArrayItem(pointatt, i)->valuestring;
            if(data_type == "POSITION_CARTESIAN") {
                info->pointAttributes.push_back(POSITION_CARTESIAN);
                info->pointByteSize += 3 * sizeof(float);
            }
            else if(data_type == "COLOR_PACKED") {
                info->pointAttributes.push_back(COLOR_PACKED);
                info->pointByteSize += 4 * sizeof(char);
            }
            else {
                cout << "Invalid data type" << endl;
                return -1;
            }
        }
        cout << endl;

        info->spacing = cJSON_GetObjectItem(json, "spacing")->valuedouble;
        info->scale = cJSON_GetObjectItem(json, "scale")->valuedouble;
        info->hierarchyStepSize = cJSON_GetObjectItem(json, "hierarchyStepSize")->valueint;

        // other settings
        info->dataDir = data_dir;

        info->boundingBoxCentre[0] = (info->boundingBox[0] + info->boundingBox[3]) / 2;
        info->boundingBoxCentre[1] = (info->boundingBox[1] + info->boundingBox[4]) / 2;
        info->boundingBoxCentre[2] = (info->boundingBox[2] + info->boundingBox[5]) / 2;
        //for(int i=0; i < 3; i++)
        //  info.boundingBoxCentre[i] = info.boundingBox[i];
    }

    cJSON_Delete(json);
    delete [] data;

    return 0;
}

void Utils::printPCInfo(const PCInfo* info) {
    cout << "==== PC Info ====" << endl;
    cout << "Version: " << info->version << endl;
    cout << "dataDir: " << info->dataDir << endl;
    cout << "octreeDir: " << info->octreeDir << endl;
    cout << "boundingBox: ";
    for(int i=0; i < 6; i++)
        cout << info->boundingBox[i] << " ";
    cout << endl;

    cout << "tightBoundingBox: ";
    for(int i=0; i < 6; i++)
        cout << info->tightBoundingBox[i] << " ";
    cout << endl;

    cout << "pointAttributes: ";
    for(int i=0; i < info->pointAttributes.size(); i++)
        cout << info->pointAttributes[i] << " ";
    cout << endl;

    cout << "Spacing: " << info->spacing << endl;
    cout << "Scale: " << info->scale << endl;
    cout << "hierarchyStepSize: " << info->hierarchyStepSize << endl << endl;
}

void Utils::addVectors(const float v1[3], const float v2[3], float v[3]) {
    for(int i=0; i < 3; i++)
        v[i] = v1[i] + v2[i];
}

void Utils::addVectors(const float v1[3], const float v2[3], const float v3[3], float v[3]) {
    for(int i=0; i < 3; i++)
        v[i] = v1[i] + v2[i] + v3[i];
}

int Utils::createChildAABB(const float pbbox[6], const int childIndex, float cbbox[6]) {
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

