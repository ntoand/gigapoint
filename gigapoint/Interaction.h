#ifndef _INTERACTION_H_
#define _INTERACTION_H_

#include "omega.h"
#include "Utils.h"

namespace gigapoint {


class FractureTracer;
class PointCloud;

class Interaction {
public:
    Interaction(PointCloud *p);
    ~Interaction();

    // interaction
    void updateRay(const omega::Ray& r);
    void findHitPoint();
    void traceFracture();
    void updateInteractionMode(const string &mode);
    void draw();

private:
    PointCloud* m_cloud;

    // interaction
    omega::Ray ray;
    vector<HitPoint*> hitPoints;
    int interactMode;
    //fracture tracing
    FractureTracer* tracer;

}; //class Interaction
}; // namespace

#endif //_INTERACTION_H_

