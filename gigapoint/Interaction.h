#ifndef _INTERACTION_H_
#define _INTERACTION_H_

#include "omega.h"
#include "Utils.h"
#include "iostream"

namespace gigapoint {


class FractureTracer;
class PointCloud;

class Interaction {
public:
    Interaction(PointCloud *p);
    ~Interaction();

    // interaction
    void updateRay(const omega::Ray& r, int playerid);
    bool findHitPoint(int playerid);
    void traceFracture(int playerid);
    void traceAllFractures();
    //void insertTracePoint(const omega::Vector3f &origin,const omega::Vector3f &direction,int playerid);
    void updateInteractionMode(const string &mode);
    void draw();
    void drawTrace();
    void setDrawTrace(bool b){m_drawTrace=b;}
    void next();
    void pickPointFromRay(const omega::Vector3f &origin,const omega::Vector3f &direction,int playerid);
    void useSelectedPointAsTracePoint();
    void resetTracer(int playerid);
    int test(int playerID);
    void setTracerPointScale(float scale);
    FractureTracer *getTracer(int playerid) {return m_tracers[playerid];}
    void setColor(std::string,std::string,float r,float g,float b);

private:
    bool m_drawTrace;
    void drawCrosshair();
    int windowWidth,windowHeight;
    PointCloud* m_cloud;

    // interaction
    omega::Ray rays[3];
    std::pair<bool,HitPoint> selectionPoints[3];
    vector<HitPoint*> hitPoints;

    int interactMode;
    //fracture tracing
    FractureTracer* tracerRED;
    FractureTracer* tracerGREEN;
    FractureTracer* tracerBLUE;
    FractureTracer* tracer;
    std::vector< FractureTracer* > m_tracers;
    void setTracerByPlayerId(int playerid);

}; //class Interaction
}; // namespace

#endif //_INTERACTION_H_

