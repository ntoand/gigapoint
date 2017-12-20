#include "Interaction.h"
#include "PointCloud.h"
#include "FractureTracer.h"
#include "NodeGeometry.h"

using namespace std;
using namespace omicron;

namespace gigapoint {
Interaction::Interaction(PointCloud *p): m_cloud(p) ,interactMode(INTERACT_NONE),
    tracerRED(NULL),tracerGREEN(NULL),tracerBLUE(NULL),tracer(NULL),m_drawTrace(true),numSegmentTracerThread(9)
{
    windowWidth=m_cloud->getWidth();
    windowHeight=m_cloud->getHeight();
    m_tracers.clear();
    tracerRED=new FractureTracer(m_cloud,1,this);
    tracerGREEN=new FractureTracer(m_cloud,2,this);
    tracerBLUE=new FractureTracer(m_cloud,3,this);
    m_tracers.push_back(tracerRED);
    m_tracers.push_back(tracerGREEN);
    m_tracers.push_back(tracerBLUE);


    if(segmentTracerThreads.size() == 0) {
        for(int i = 0; i < numSegmentTracerThread; i++) {
            SegmentTracerThread* t = new SegmentTracerThread(segmentQueue);
            t->start();
            segmentTracerThreads.push_back(t);
            cout << "starting segment thread. ID:";
        }

    }

}

Interaction::~Interaction() {
    for (int i=0;i<3;i++)
    {
        delete m_tracers[i];
    }
    /*
    if(tracerRED)
        delete tracerRED;
    if(tracerGREEN)
        delete tracerGREEN;
    if(tracerBLUE)
        delete tracerBLUE;
    */
}

void Interaction::setTracerByPlayerId(int playerid) {
    switch (playerid) {
            case 1: if (tracerRED==NULL) {tracerRED=new FractureTracer(m_cloud,1,this); m_tracers[0]=tracerRED;} ;tracer=tracerRED;break;
            case 2: if (tracerGREEN==NULL) {tracerGREEN=new FractureTracer(m_cloud,2,this);m_tracers[1]=tracerGREEN;} ;tracer=tracerGREEN; break;
            case 3: if (tracerBLUE==NULL) {tracerBLUE=new FractureTracer(m_cloud,3,this);m_tracers[2]=tracerBLUE;} ;tracer=tracerBLUE; break;
            default: std::cout << "unhandled playerid" << playerid << std::endl;
        }
}

void Interaction::resetTracer(int playerid)
{
    setTracerByPlayerId(playerid);
    tracer->destroy();

}

int Interaction::test(int playerID) {
    setTracerByPlayerId(playerID);
    return tracer->test(playerID);
    /*
    NodeGeometry *node;
    std::cout << "intertest" << std::endl;
    m_cloud->tryGetNode("r4",node);
    //std::cout << b << node << std::endl;
    PointIndex p1(node,7719);
    PointIndex p2(node,7719);
    // compare p1 and p2 should be different adress
    std::map< PointIndex,int > m;
    m[p1]=1;
    m.erase(p2);
    int j=7;
    */
}

void Interaction::setTracerPointScale(float scale)
{
    for (int i=0;i<3;i++)
    {
        if (NULL!=m_tracers[i]) {
            m_tracers[i]->setPointScale(scale);
            cout << "Updating Tracer Scale to: " << scale << endl;
        }
    }
}

//void Interaction::next() { tracer->next(); }

void Interaction::draw()
{
    // draw interaction
    if(interactMode != INTERACT_NONE) {        

        //draw ray line
        Vector3f spos;
        Vector3f epos;
        omega::Ray ray;
        for (int playerid=0;playerid<3;playerid++)
        {
            glDisable(GL_LIGHTING);
            glDisable(GL_BLEND);
            ray=rays[playerid];
            spos = ray.getOrigin(); //- 1*ray.getDirection();
            epos = ray.getOrigin() + 100*ray.getDirection();
            glLineWidth(4.0);
            glColor3f(m_tracers[playerid]->rayColor[0], m_tracers[playerid]->rayColor[1], m_tracers[playerid]->rayColor[2]);
            glBegin(GL_LINES);
            glVertex3f(spos[0], spos[1], spos[2]);
            glVertex3f(epos[0], epos[1], epos[2]);
            glEnd();
            if (selectionPoints[playerid].first) {
                glEnable(GL_PROGRAM_POINT_SIZE_EXT);
                glPointSize(20);
                glColor3f(m_tracers[playerid]->selectionColor[0], m_tracers[playerid]->selectionColor[1], m_tracers[playerid]->selectionColor[1]);
                glBegin(GL_POINTS);
                glVertex3f(selectionPoints[playerid].second.position[0], selectionPoints[playerid].second.position[1], selectionPoints[playerid].second.position[2]);
                glEnd();
            }
        }

    }
    drawTrace();
}

void Interaction::drawTrace()
{
    if (!m_drawTrace)
        return;
    if (tracerRED!=NULL)
        if (tracerRED->destroyme()) {
            delete tracerRED;
            tracerRED=NULL;
        } else { tracerRED->render();}
    if (tracerGREEN!=NULL)
        if (tracerGREEN->destroyme()) {
            delete tracerGREEN;
            tracerGREEN=NULL;
        } else { tracerGREEN->render();}
    if (tracerBLUE!=NULL)
        if (tracerBLUE->destroyme()) {
            delete tracerBLUE;
            tracerBLUE=NULL;
        } else { tracerBLUE->render();}
}

void Interaction::useSelectedPointAsTracePoint(int playerid)
{
    if(!selectionPoints[playerid-1].first)
	return;

    //if (hitPoints.size()==0)
    //    return;
    //cout << "Inserting new Tracepoint Node / PointIndex:" << hitPoints[0]->node->getName() << " / " << hitPoints[0]->index << std::endl;
    Point p1(selectionPoints[playerid-1].second.node,selectionPoints[playerid-1].second.index);
    p1.index.node->getPointData(p1);
    tracer->insertWaypoint(p1);
}


void Interaction::pickPointFromRay(const omega::Vector3f &origin,const omega::Vector3f &direction,int playerid) {
    setTracerByPlayerId(playerid);
    rays[playerid-1].setOrigin(origin);
    rays[playerid-1].setDirection(direction);
    findHitPoint(playerid);
}



// interaction
void Interaction::updateRay(const omega::Ray& r, int playerid) {
    rays[playerid-1] = r;
}


void Interaction::traceAllFractures()
{
    /*
    if (tracerRED!=NULL)
        tracerRED->update();
    if (tracerGREEN!=NULL)
        tracerGREEN->update();
    if (tracerBLUE!=NULL)
        tracerBLUE->update();
        */
    //setTracerByPlayerId(1);
    //tracer->update();
}

void Interaction::traceFracture(int playerid)
{

    setTracerByPlayerId(playerid);
    if (tracer->waypoint_count()<2)
    {
        cout << "not tracing, not enough waypoints" << endl;
        return;
    }    
    tracer->optimizePath();
    //bool success = true;//tracer->optimizePath(1000000);
    //if (success)
//        cout << "Tracing was sucessfull #Tracepoints:" << tracer->getTraceRef().size() << endl;
  //  else
    //    cout << "Tracing was not sucessfull" << endl;
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

bool hitpointcomparator (HitPoint* i,HitPoint* j) { return (i->distance<j->distance); }

bool Interaction::findHitPoint(int playerid) {

    if (interactMode == INTERACT_POINT) {
        omega::Ray ray = rays[playerid-1];
        unsigned int start_time = Utils::getTime();
        hitPoints.clear();
        HitPoint* point = new HitPoint();
        std::list<NodeGeometry*> displayList = m_cloud->getListOfVisibleNodesRef();
        for(list<NodeGeometry*>::iterator it = displayList.begin(); it != displayList.end(); it++) {
            NodeGeometry* node = *it;
            node->findHitPoint(ray, point);            
        }
        hitPoints.push_back(point);
        std::sort (hitPoints.begin(), hitPoints.end(), hitpointcomparator);
        if(point->distance != -1) {
            hitPoints.push_back(point);
            selectionPoints[playerid-1]=std::pair<bool,HitPoint>(true,HitPoint(hitPoints[0]));
            cout << "find time: " << Utils::getTime() - start_time << " size: " << hitPoints.size()
                << " dis: " << hitPoints[0]->distance
                << " pos: " << hitPoints[0]->position[0] << " " << hitPoints[0]->position[1] << " "
                << hitPoints[0]->position[2] << endl;
           /* if (hitPoints.size() >=2 ) {
                float dis=Utils::distance(hitPoints[0]->position,hitPoints[1]->position);
                cout << "distance between first and second point is: " << dis << endl;
                cout << hitPoints[0]->node->getName() << " " << hitPoints[0]->index << std::endl;
                cout << hitPoints[0]->distance << " " << hitPoints[hitPoints.size()-1]->distance << endl;
            }*/

        }
        else {
            cout << "MISS" << endl;
            hitPoints.clear();
            selectionPoints[playerid-1]=std::pair<bool,HitPoint>(false,HitPoint());
        }
    }
}

void Interaction::setColor(std::string tracername, std::string component,float r,float g,float b)
{

    FractureTracer *_tracer=NULL;
    if (tracername == "RED") {_tracer=tracerRED;}
    else if (tracername=="GREEN"){_tracer=tracerGREEN;}
    else if (tracername=="BLUE"){_tracer=tracerBLUE;}
    else{cout << "cannot set color for tracer with name: "<< tracername <<endl;return;}

    if (_tracer!=NULL)
        _tracer->setComponentColor(component,r,g,b);
}

void Interaction::addSegmentToTracingQueue(TracingSegment *segment)
{
    segmentQueue.add(segment);
}


} // namespace Interaction
