
#include "TracingSegment.h"
#include "PointCloud.h"
#include "FractureTracer.h"
#include "Utils.h"

#include <map>

namespace gigapoint {

    TracingSegment::TracingSegment(FractureTracer *tracer_ ,Point start_,Point end_) :
        status(SEGMENT_INQUEUE),start(start_),end(end_),m_end_rgb(end_),m_start_rgb(start_),tracer(tracer_) {
    }

    void TracingSegment::trace()
    {
        cout << "tracing a segment " << endl;
        iter_count=0;

        PointCloud* cloud=tracer->getCloud();
        float search_r=tracer->getSearchRadius();
        unsigned int start_time = Utils::getTime();
        unsigned int tic;
        unsigned int neighbour_search_time=0;
        int cost=0;

        int smallest_cost= MAX_COST;
        int m_maxIterations=10000;
        Point current;

        std::map<PointIndex, PointData> m_PointData;
        std::map<PointIndex, PointData>::iterator it_pointdata;
        std::map<PointIndex, PointIndex> m_openSet; //nodes we are currently exploring (key=nodeID, value=prevNodeID)
        std::vector< Point > neighbours;

        m_start_rgb= start;
        m_end_rgb= end;

        if(status==SEGMENT_START) {   // initialisation, or first time run
            segmentPath.clear();
            m_PointData.clear();
            //m_closedSet.clear(); //visited nodes (key=nodeID, value=prevNodeID)
            m_openSet.clear(); //nodes we are currently exploring (key=nodeID, value=prevNodeID)
            //m_dist.clear(); //<node, cost estimate from start to end via this node>

            m_openSet[start.index] = start.index; //open start node

            //m_dist[start.index] = 0; //start-start distance = 0
            //m_dist[end.index] = MAX_COST;
            m_PointData[start.index]=PointData();
            m_PointData[start.index].cost=0;
            m_PointData[start.index].previous=start.index;
            m_PointData[start.index].visited=true;
            m_PointData[end.index].cost=MAX_COST;
            //m_initialized=true;
            status =SEGMENT_TRACING;
        }


        //code essentialy taken from wikipedia page for Djikstra: https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm

        //declare variables used in the loop
        float cur_d2, next_d2;

        if (m_openSet.size()==0)
        {
            cout << "openSet is empty. we seem to be finished." <<endl;
            status =SEGMENT_FINISHED_NOTFOUND;
            return;
        }
        while (m_openSet.size() > 0) //while unvisited nodes exist
        {


            //check if we excede max iterations
            if (iter_count > m_maxIterations) {
                cout << "[FractureTracer::optimizeSegment] max iterations reached. Stopping" << iter_count << " " << m_maxIterations<< std::endl;
                return ; //bail
            }
            iter_count++;

            //get node from openSet with smallest cost
            smallest_cost = MAX_COST;
            for (std::map<PointIndex, PointIndex>::iterator it=m_openSet.begin();it!=m_openSet.end();++it){
                //m_cost = m_dist[(*it).first];
                cost = m_PointData[(*it).first].cost;
                if (cost < smallest_cost)
                {
                    smallest_cost = cost;
                    current =cloud->getPointFromIndex((*it).first);
                }
            }

            it_pointdata = m_PointData.find(current.index);
            if (it_pointdata != m_PointData.end())
            {
                (*it_pointdata).second.visited=true;
            } else
            {
                m_PointData[current.index]=PointData(true);
                m_PointData[current.index].visited=true;
            }

            m_openSet.erase(current.index); //remove current from openSet
            //cout << "m_current pos:" << m_current.position[0] << " index "<<m_current.index.index << std::endl;
            if (current.index == end.index) //we've found it!
            {
                segmentPath.push_back(end); //add end node

                //traverse backwards to reconstruct path
                current = cloud->getPointFromIndex(m_PointData[current.index].previous); //move back one
                while (current.index != start.index)
                {
                    segmentPath.push_front(current);
                    current = cloud->getPointFromIndex(m_PointData[current.index].previous);
                }
                segmentPath.push_front(start);

                //return
                cout << "[FractureTracer::optimizeSegment] finished after iterations:" << iter_count << std::endl;
                status = SEGMENT_FINISHED_FOUND;
                return;
            }

            //calculate distance from current nodes parent to end -> avoid going backwards (in euclidean space) [essentially stops fracture turning > 90 degrees)
            Point cur = current;//m_cloud->getPoint(closedSet[current]);
            cur_d2 =	(cur.position[0] - end.position[0])*(cur.position[0] - end.position[0]) +
                        (cur.position[1] - end.position[1])*(cur.position[1] - end.position[1]) +
                        (cur.position[2] - end.position[2])*(cur.position[2] - end.position[2]);

            //fill "neighbours" with nodes - essentially get results of a "sphere" search around active current point
            tic = Utils::getTime();
            neighbours.clear();
            neighbours=cloud->getPointsInSphericalNeighbourhood(current, search_r);
            //cout << "[FractureTracer::optimizeSegment] getPointsInSphericalNeighbourhood returned :" << m_neighbours.size() << std::endl;

            if (neighbours.size() > 10000) {
                search_r/=2;
                cout << "new m_search_r: " << search_r <<endl;
            }
            else if (neighbours.size()< 100 ) {
                while (neighbours.size() < 100 ) {
                    search_r*=2;
                    neighbours=cloud->getPointsInSphericalNeighbourhood(current, search_r);
                    cout << "new m_search_r: " << search_r <<" researching" <<endl;
                }
            }


            neighbour_search_time+=Utils::getTime()-tic;
            /*
            if (omega::SystemManager::instance()->isMaster()) {
                cout << "Iter / Neigh / search_r / neigh_time / dist2start / dist2end : " << m_iter_count << " / " << m_neighbours.size() << " / " << m_search_r
                    << " / " << neighbour_search_time << " / " << Utils::distance(start.position,m_current.position) << " / " << Utils::distance(end.position,m_current.position) << endl;
            }
            */


            //loop through neighbours
            for (size_t i = 0; i < neighbours.size(); i++) //N.B. i = [pointID,cost]
            {
                Point apoint = neighbours[i];

                //calculate (squared) distance from this neighbour to the end
                next_d2 =	(apoint.position[0] - end.position[0])*(apoint.position[0] - end.position[0]) +
                            (apoint.position[1] - end.position[1])*(apoint.position[1] - end.position[1]) +
                            (apoint.position[2] - end.position[2])*(apoint.position[2] - end.position[2]);
                //dijkstradebug
                if (next_d2 >= cur_d2) //Bigger than the original distance? If so then bail.
                    continue;

                it_pointdata = m_PointData.find(apoint.index);
                if (it_pointdata==m_PointData.end())
                {
                    m_PointData[apoint.index]=PointData();
                    it_pointdata = m_PointData.find(apoint.index);
                }

                if ((*it_pointdata).second.visited==true) //Is point in closed set? If so, bail.
                    continue;

                //calculate cost to this neighbour
                cost = getSegmentCostRGB(current, apoint);

                //transform into cost from start node
                cost += m_PointData[current.index].cost;
                m_openSet[apoint.index] = apoint.index; //add to open set - we can move here next

                //have we visited this node before?
                if ((*it_pointdata).second.cost!=-1)
                {
                    //is this cost better?
                    if (cost < (*it_pointdata).second.cost)
                    {
                        //m_dist[apoint.index] = m_cost; //update cost with better value
                        (*it_pointdata).second.cost=cost;
                        (*it_pointdata).second.previous=current.index;//store this as the best path to this node
                    }
                    else
                        continue; //skip
                } else //this is the first time we've looked at this node
                {

                    (*it_pointdata).second.cost=cost;
                    (*it_pointdata).second.previous=current.index;
                }
            }
        } // while
        cout << "[FractureTracer::optimizeSegment] iterations:" << iter_count << std::endl;
        //assert(false);
        status = SEGMENT_FINISHED_NOTFOUND;
        return;
    }

    int TracingSegment::getSegmentCostRGB(Point p1_rgb, Point p2_rgb)
    {
        //get colors
        //const ColorCompType* p1_rgb = m_cloud->getPointColor(p1);
        //const ColorCompType* p2_rgb = m_cloud->getPointColor(p2);

        //cost function: cost = |c1-c2| + 0.25 ( |c1-start| + |c1-end| + |c2-start| + |c2-end| )
        //IDEA: can we somehow optimize all the square roots here (for speed)?
        return sqrt(
            //|c1-c2|
            (p1_rgb.color[0] - p2_rgb.color[0]) * (p1_rgb.color[0] - p2_rgb.color[0]) +
            (p1_rgb.color[1] - p2_rgb.color[1]) * (p1_rgb.color[1] - p2_rgb.color[1]) +
            (p1_rgb.color[2] - p2_rgb.color[2]) * (p1_rgb.color[2] - p2_rgb.color[2])) + 0.25 * (
            //|c1-start|
            sqrt((p1_rgb.color[0] - m_start_rgb.color[0]) * (p1_rgb.color[0] - m_start_rgb.color[0]) +
            (p1_rgb.color[1] - m_start_rgb.color[1]) * (p1_rgb.color[1] - m_start_rgb.color[1]) +
            (p1_rgb.color[2] - m_start_rgb.color[2]) * (p1_rgb.color[2] - m_start_rgb.color[2])) +
            //|c1-end|
            sqrt((p1_rgb.color[0] - m_end_rgb.color[0]) * (p1_rgb.color[0] - m_end_rgb.color[0]) +
            (p1_rgb.color[1] - m_end_rgb.color[1]) * (p1_rgb.color[1] - m_end_rgb.color[1]) +
            (p1_rgb.color[2] - m_end_rgb.color[2]) * (p1_rgb.color[2] - m_end_rgb.color[2])) +
            //|c2-start|
            sqrt((p2_rgb.color[0] - m_start_rgb.color[0]) * (p2_rgb.color[0] - m_start_rgb.color[0]) +
            (p2_rgb.color[1] - m_start_rgb.color[1]) * (p2_rgb.color[1] - m_start_rgb.color[1]) +
            (p2_rgb.color[2] - m_start_rgb.color[2]) * (p2_rgb.color[2] - m_start_rgb.color[2])) +
            //|c2-end|
            sqrt((p2_rgb.color[0] - m_end_rgb.color[0]) * (p2_rgb.color[0] - m_end_rgb.color[0]) +
            (p2_rgb.color[1] - m_end_rgb.color[1]) * (p2_rgb.color[1] - m_end_rgb.color[1]) +
            (p2_rgb.color[2] - m_end_rgb.color[2]) * (p2_rgb.color[2] - m_end_rgb.color[2]))) / 3.5; //N.B. the divide by 3.5 scales this cost function to range between 0 & 255
    }



}; // namespace gigapoint
