#include "Interaction.h"
#include "PointCloud.h"
#include "FractureTracer.h"
#include "NodeGeometry.h"

using namespace std;
using namespace omicron;

namespace gigapoint {
Interaction::Interaction(PointCloud *p): m_cloud(p) ,interactMode(INTERACT_NONE),tracer(NULL)
{

}

Interaction::~Interaction() {
    if(tracer)
        delete tracer;
}

void Interaction::draw()
{
    // draw interaction
    if(interactMode != INTERACT_NONE) {
        glDisable(GL_LIGHTING);
        glDisable(GL_BLEND);
        //draw ray line
        Vector3f spos = ray.getOrigin(); //- 1*ray.getDirection();
        Vector3f epos = ray.getOrigin() + 100*ray.getDirection();
        glLineWidth(4.0);
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        glVertex3f(spos[0], spos[1], spos[2]);
        glVertex3f(epos[0], epos[1], epos[2]);
        glEnd();

        glEnable(GL_PROGRAM_POINT_SIZE_EXT);
        glPointSize(40);
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_POINTS);
        for(int i=0; i < hitPoints.size(); i++) {
            glVertex3f(hitPoints[i]->position[0], hitPoints[i]->position[1], hitPoints[i]->position[2]);
        }
        glEnd();
    }
}

// interaction
void Interaction::updateRay(const omega::Ray& r) {
    ray = r;
}

void Interaction::traceFracture()
{
    if (tracer==NULL)
    {
        tracer= new FractureTracer(m_cloud);
    }
    NodeGeometry* root=NULL;
    m_cloud->tryGetNode("r",root);
    Point p1(root,4807);
    Point p2(m_cloud->getRoot(),5795);
    p1.index.node->getPointData(p1);
    p2.index.node->getPointData(p2);

    tracer->insertWaypoint(p1);
    tracer->insertWaypoint(p2);
    tracer->optimizePath();
    std::vector < std::deque<Point > > trace=tracer->getTraceRef();
    for (std::vector < std::deque<Point > >::iterator it1 = trace.begin() ; it1 != trace.end(); ++it1)
    {
        for (std::deque<Point >::iterator it2 = (*it1).begin() ; it2 != (*it1).end(); ++it2)
        {
            (*it2).index.node->setPointColor((*it2),1,254,1);
        }
    }
    root->initVBO();
}


void Interaction::updateInteractionMode(const string& mode)
{
    if(mode.compare("None")==0)
        interactMode=INTERACT_NONE;
    else if(mode.compare("Point")==0)
        interactMode=INTERACT_POINT;
    else if(mode.compare("MultiFracture")==0)
        interactMode=INTERACT_MULTIFRACTURE;
    else
        std::cerr << "Unable to set pointcloud::interact mode to "<< mode << " Supported modes are [None | Point | MultiFracture]" << std::endl;
}

void Interaction::findHitPoint() {

    if (interactMode == INTERACT_POINT) {
        unsigned int start_time = Utils::getTime();
        hitPoints.clear();
        HitPoint* point = new HitPoint();
        std::list<NodeGeometry*> displayList = m_cloud->getListOfVisibleNodesRef();
        for(list<NodeGeometry*>::iterator it = displayList.begin(); it != displayList.end(); it++) {
            NodeGeometry* node = *it;
            node->findHitPoint(ray, point);
            hitPoints.push_back(point);
        }

        if(point->distance != -1) {
            hitPoints.push_back(point);
            cout << "find time: " << Utils::getTime() - start_time << " size: " << hitPoints.size()
                << " dis: " << hitPoints[0]->distance
                << " pos: " << hitPoints[0]->position[0] << " " << hitPoints[0]->position[1] << " "
                << hitPoints[0]->position[2] << endl;
            if (hitPoints.size() >=2 ) {
                float dis=Utils::distance(hitPoints[0]->position,hitPoints[1]->position);
                cout << "distance between first and second point is: " << dis << endl;
            }
        }
        else {
            cout << "MISS" << endl;
        }
    }
}


} // namespace Interaction
