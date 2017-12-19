//##########################################################################
//#                                                                        #
//#                    CLOUDCOMPARE PLUGIN: ccCompass                      #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                     COPYRIGHT: Sam Thiele  2017                        #
//#                                                                        #
//##########################################################################

#include "omega.h"

#include "FractureTracer.h"
#include "PointCloud.h"
#include "iostream"
#include "NodeGeometry.h"
#include "TracingSegment.h"
#include "Interaction.h"

namespace gigapoint {

FractureTracer::FractureTracer(PointCloud* cloud,int playerid,Interaction* inter) : m_relMarkerScale(5.0f),m_cloud(NULL),m_previous(-1),
    m_waitforinput(false),tracerstatus(DONOTHING),m_destroy(false),pointscale(20),m_playerid(playerid),interactionClass(inter),m_search_r(2) {
    selectionColor[0]=0.0;
    selectionColor[1]=1.0;
    selectionColor[2]=0.0;
    rayColor[0]=0.0;
    rayColor[1]=0.0;
    rayColor[2]=0.0;
    //m_initialized=false;
    //m_finishedTrace=false;
    m_maxIterations=100000;

    m_cloud = cloud; //store pointer ourselves also
    //m_search_r = calculateOptimumSearchRadius(); //estimate the search radius we want to use    
    //m_search_r = 2;
    //std::cout << " Fracture tracer search radius/spacing : " << m_search_r <<" / " << m_cloud->getPCInfo()->spacing << std::endl;
	//store these info as object attributes
	//object->hasMetaData("search_r") && object->hasMetaData("cost_function");
    /*
    QVariantMap* map = new QVariantMap();
	map->insert("search_r", m_search_r); 
	QString cost_function = "";
	if (COST_MODE & MODE::RGB)
		cost_function += "RGB,";
	if (COST_MODE & MODE::DARK)
		cost_function += "Dark,";
	if (COST_MODE & MODE::LIGHT)
		cost_function += "Light,";
	if (COST_MODE & MODE::CURVE)
		cost_function += "Curve,";
	if (COST_MODE & MODE::GRADIENT)
		cost_function += "Grad,";
	if (COST_MODE & MODE::DISTANCE)
		cost_function += "Dist,";
	if (COST_MODE & MODE::SCALAR)
		cost_function += "Scalar,";
	if (COST_MODE & MODE::INV_SCALAR)
		cost_function += "Inv_Scalar,";
	cost_function = cost_function.remove(cost_function.size() - 1, 1); //remove trailing comma
    map->insert("cost_function", cost_function);
    setMetaData(*map, true);
    */

}

void FractureTracer::checkOnTraces() {
    std::vector<TracingSegment *>::iterator it=m_trace.begin();
    TracingSegment *seg;
    bool success=true;
    for( ; it != m_trace.end();it++)
    {
        seg=(*it);
        if (seg->getStatus()<=3)
            return;
        else if(seg->getStatus()==TracingSegment::SEGMENT_FINISHED_NOTFOUND)
            success=false;
    }
    //if (!success)
    //    m_trace.clear();
    setTracerStatus(FINISHED_DONE);
}

bool FractureTracer::optimizePath()
{
    if (m_waypoints.size() < 2)
	{
		m_trace.clear();
		return false; //no segments...
	}

	//loop through segments and build/rebuild trace
    Point start, end;
    int tID; //declare vars

    if (m_search_r = -1)
    {
        m_search_r = Utils::distance(m_waypoints[0].position,m_waypoints[m_waypoints.size()-1].position)/20;
    }

	for (unsigned i = 1; i < m_waypoints.size(); i++)
	{
		//calculate indices
		start = m_waypoints[i - 1]; //global point id for the start waypoint
		end = m_waypoints[i]; //global point id for the end waypoint
        tID = i - 1; //id of the trace segment id (in m_trace vector)
        TracingSegment *seg = new TracingSegment(this,m_waypoints[i - 1],m_waypoints[i]);
        m_trace.push_back(seg);
        interactionClass->addSegmentToTracingQueue(seg);

        /*
        //TODO query status, trennen nach init und finieshed

        std::deque<Point> segment = optimizeSegment(m_waypoints[i - 1], m_waypoints[i]);


        if (tracerstatus==FINISHED_SEGMENTFOUND)
            m_trace.push_back(segment);
        else if (tracerstatus==FINISHED_SEGMENTNOTFOUND)
            cout << "tracing doesn't work" << endl;
        */
        /*
		//are we adding to the end of the trace?
		if (tID >= m_trace.size()) 
		{
            std::deque<Point> segment = optimizeSegment(start, end); //calculate segment
            if (segment.size()==0){
                cout << "invalid segment" << endl;
                return false;
            }
			m_trace.push_back(segment); //store segment

		} else //no... we're somewhere in the middle - update segment if necessary
		{
			if (!m_trace[tID].empty() && (m_trace[tID][0] == start) &&  (m_trace[tID][m_trace[tID].size() - 1] == end)) //valid trace and start/end match
				continue; //this trace has already been calculated - we can skip! :)
			else
			{
				//calculate segment
                std::deque<Point> segment = optimizeSegment(start, end); //calculate segment

				//add trace
				if (m_trace[tID][m_trace[tID].size() - 1] == end) //end matches - we can replace the current trace & things should be sweet (all prior traces will have been updated already)
					m_trace[tID] = segment; //end is correct - overwrite this block, then hopefully we will match in the next one
				else //end doesn't match - we need to insert
					m_trace.insert(m_trace.begin()+tID, segment);
			}
        } */
	}
    setTracerStatus(TRACING_SEGMENTS);
    return true;
}



int FractureTracer::insertWaypoint(Point Q)
{

    if (m_waypoints.size() >= 2)
    {
        //get location of point to add
        //const CCVector3* Q = m_cloud->getPoint(pointId);
        //float Q[3] = *m_cloud->getPoint(pointId);
        Point start, end;
        //check if point is "inside" any segments
        for (int i = 1; i < m_waypoints.size(); i++)
        {
            //get start and end points
            //m_cloud->getPoint(m_waypoints[i - 1], start);
            //m_cloud->getPoint(m_waypoints[i], end);
            start=m_waypoints[i-1];
            end=m_waypoints[i];

            //are we are "inside" this segment
            if (Utils::inCircle(start.position, end.position, Q.position))
            {
                //insert waypoint
                m_waypoints.insert(m_waypoints.begin() + i, Q);
                m_previous = i;
                return i;
            }
        }

        //check if the point is closer to the start than the end -> i.e. the point should be 'pre-pended'
        //CCVector3 sp = Q - start;
        //CCVector3 ep = Q - end;
        float ds=DIST3(Q.position,start.position);
        float de=DIST3(Q.position,end.position);
        if (ds < de)
        {
            m_waypoints.insert(m_waypoints.begin(), Q);
            m_previous = 0;
            return 0;
        }
    }

    //add point to end of the trace
    m_waypoints.push_back(Q);
    m_previous = static_cast<int>(m_waypoints.size()) - 1;
    return m_previous;
}


#if 0

void FractureTracer::_printOpenSet()
{
    cout << "OpenSet:" << std::endl;
    for (std::map<PointIndex, PointIndex>::iterator it=m_openSet.begin();it!=m_openSet.end();++it){
        Point apoint  =m_cloud->getPointFromIndex((*it).first);
        std::cout << "openst_add Node/IDx/Pos: " << apoint.index.node->getName() <<"/" << apoint.index.index << " "
                  << apoint.position[0] << " " << apoint.position[1] << " "  << apoint.position[2] << std::endl;
    }

}

void FractureTracer::_print() {
    for (it_pointdata=m_PointData.begin(); it_pointdata!=m_PointData.end(); ++it_pointdata)
    {
        std::cout << "PData: "<<m_cloud->getPointFromIndex(it_pointdata->first).position[0] << " Visited: "
                              << it_pointdata->second.visited << " " << m_cloud->getPointFromIndex(it_pointdata->first).color[0];
        if (-1==it_pointdata->second.cost || MAX_COST==it_pointdata->second.cost)
              std::cout << std::endl;
        else if (0==it_pointdata->second.cost)
            std::cout << " Cost: " << it_pointdata->second.cost << std::endl;
        else {
            std::cout << " Cost: " << it_pointdata->second.cost << " Prev. Position: "
                      << m_cloud->getPointFromIndex(it_pointdata->second.previous).position[0] << std::endl ;
        }
    }
 }


//int FractureTracer::COST_MODE = FractureTracer::MODE::DARK; //set default cost mode


std::deque<Point> FractureTracer::optimizeSegment(Point start, Point end, int maxIterations)
{

	//check handle to point cloud
    if (!m_cloud)
	{
        return std::deque<Point>(); //error -> no cloud
	}

	//retreive and store start & end rgb
    /*if (m_path.size()>0) {  //REMOVE
        m_start_rgb= m_path[0];
        start=m_path[0];
    } else */
    m_start_rgb= start;
    m_end_rgb= end;
    m_path.clear();
    //const ColorCompType* s = m_cloud->getPointColor(start);
    //const ColorCompType* e = m_cloud->getPointColor(end);
    //m_start_rgb[0] = s[0]; m_start_rgb[1] = s[1]; m_start_rgb[2] = s[2];
    //m_end_rgb[0] = e[0]; m_end_rgb[1] = e[1]; m_end_rgb[2] = e[2];


	//get location of target node - used to optimise algorithm to stop searching paths leading away from the target
    //const CCVector3* end_v = m_root->getPoint(end);

	//code essentialy taken from wikipedia page for Djikstra: https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm
    std::map<PointIndex, PointIndex> closedSet; //visited nodes (key=nodeID, value=prevNodeID)
    std::map<PointIndex, PointIndex> openSet; //nodes we are currently exploring (key=nodeID, value=prevNodeID)
    std::map<PointIndex, int> dist; //<node, cost estimate from start to end via this node>

	//declare variables used in the loop    

	int cost = 0;
    int smallest_cost = MAX_COST;
	int iter_count = 0;
	float cur_d2, next_d2;

	//setup octree & values for nearest neighbour searches
    //ccOctree::Shared oct = m_cloud->getOctree();
    //if (!oct)
    //{
    //	oct = m_cloud->computeOctree(); //if the user clicked "no" when asked to compute the octree then tough....
    //}
    //unsigned char level = oct->findBestLevelForAGivenNeighbourhoodSizeExtraction(search_r);

    openSet[start.index] = start.index; //open start node
    dist[start.index] = 0; //start-start distance = 0
	while (openSet.size() > 0) //while unvisited nodes exist
	{
        if (m_waitforinput)
            continue;
		//check if we excede max iterations
        if (iter_count > maxIterations) {
            cout << "[FractureTracer::optimizeSegment] max iterations reached. Stopping" <<iter_count << " " << maxIterations<< std::endl;
            return std::deque<Point>(); //bail
        }
		iter_count++;

		//get node from openSet with smallest cost
        smallest_cost = MAX_COST;
        for (std::map<PointIndex, PointIndex>::iterator it=openSet.begin();it!=openSet.end();++it){        
            cost = dist[(*it).first];
			if (cost < smallest_cost)
			{
				smallest_cost = cost;
                m_current =m_cloud->getPointFromIndex((*it).first);
			}
		}        
        closedSet[m_current.index] = openSet[m_current.index]; //add current to closedSet
        openSet.erase(m_current.index); //remove current from openSet

        if (m_current.index == end.index) //we've found it!
		{
            m_path.push_back(end); //add end node

			//traverse backwards to reconstruct path
            m_current = m_cloud->getPointFromIndex(closedSet[m_current.index]); //move back one
            while (m_current.index != start.index)
			{
                m_path.push_front(m_current);
                m_current =m_cloud->getPointFromIndex(closedSet[m_current.index]);
			}
            m_path.push_front(start);

			//return
            cout << "[FractureTracer::optimizeSegment] iterations:" << iter_count << std::endl;
            return m_path;
		}

		//calculate distance from current nodes parent to end -> avoid going backwards (in euclidean space) [essentially stops fracture turning > 90 degrees)
        Point cur = m_current;//m_cloud->getPoint(closedSet[current]);
        cur_d2 =	(cur.position[0] - end.position[0])*(cur.position[0] - end.position[0]) +
                    (cur.position[1] - end.position[1])*(cur.position[1] - end.position[1]) +
                    (cur.position[2] - end.position[2])*(cur.position[2] - end.position[2]);

		//fill "neighbours" with nodes - essentially get results of a "sphere" search around active current point
        m_neighbours.clear();
        m_neighbours=m_cloud->getPointsInSphericalNeighbourhood(m_current, m_search_r);
        //cout << "[FractureTracer::optimizeSegment] getPointsInSphericalNeighbourhood returned :" << m_neighbours.size() << std::endl;
        cout << "Iter / Neigh / search_r / dist2start / dist2end : " << iter_count << " / " << m_neighbours.size() << " / " << m_search_r
             << " / " << Utils::distance(start.position,m_current.position) << " / " << Utils::distance(end.position,m_current.position) << endl;
        if (m_neighbours.size() > 10000) {
            m_search_r/=2;
        } else if  (m_neighbours.size() < 10) {
            m_search_r*=2;
        } else if (m_neighbours.size() == 0 ) {
            while (m_neighbours.size()) {
            m_search_r*=2;
            m_neighbours=m_cloud->getPointsInSphericalNeighbourhood(m_current, m_search_r);
            }

        }

		//loop through neighbours
        for (size_t i = 0; i < m_neighbours.size(); i++) //N.B. i = [pointID,cost]
		{
            Point apoint = m_neighbours[i];

			//calculate (squared) distance from this neighbour to the end
            next_d2 =	(apoint.position[0] - end.position[0])*(apoint.position[0] - end.position[0]) +
                        (apoint.position[1] - end.position[1])*(apoint.position[1] - end.position[1]) +
                        (apoint.position[2] - end.position[2])*(apoint.position[2] - end.position[2]);

			if (next_d2 >= cur_d2) //Bigger than the original distance? If so then bail.
				continue;

            if (closedSet.count(apoint.index) == 1) //Is point in closed set? If so, bail.
				continue;

			//calculate cost to this neighbour
            cost = getSegmentCostDist(m_current, apoint);

			#ifdef DEBUG_PATH
            m_cloud->setPointScalarValue(apoint.index, static_cast<ScalarType>(cost)); //STORE VISITED NODES (AND COST) FOR DEBUG VISUALISATIONS
			#endif

			//transform into cost from start node
            cost += dist[m_current.index];

			//have we visited this node before?
            if (dist.count(apoint.index) == 1)
			{
				//is this cost better?
                if (cost < dist[apoint.index])
				{
                    dist[apoint.index] = cost; //update cost with better value
                    closedSet[apoint.index] = m_current.index; //store this as the best path to this node
                    // put render code here  REMOVE
                    //m_path.push_back(apoint);
                    //return m_path;
				}
				else
					continue; //skip
			} else //this is the first time we've looked at this node
			{
                openSet[apoint.index] = m_current.index; //add to open set - we can move here next
                dist[apoint.index] = cost; //store cost
			}
		}
    } // while
    cout << "[FractureTracer::optimizeSegment] iterations:" << iter_count << std::endl;
	assert(false);
    return std::deque<Point>(); //shouldn't come here?
}
#endif

int FractureTracer::getSegmentCostDist(Point p1, Point p2)
{
    return Utils::distance(p1.position,p2.position);
}

int FractureTracer::getSegmentCostR(Point p1, Point p2)
{
    float r1=p1.color[0];
    float r2=p2.color[0];
    if (r1==1 && r2==2) return 7;
    if (r1==1 && r2==3) return 9;
    if (r1==1 && r2==6) return 14;
    if (r1==2 && r2==3) return 10;
    if (r1==2 && r2==4) return 15;
    if (r1==3 && r2==4) return 11;
    if (r1==3 && r2==6) return 2;
    if (r1==6 && r2==5) return 9;
    if (r1==5 && r2==4) return 6;

    return MAX_COST ;
}

int FractureTracer::getSegmentCostRGB(Point p1_rgb, Point p2_rgb)
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


int FractureTracer::test(int playerID) {
    /*
    NodeGeometry* nodey;
    m_cloud->tryGetNode("r4",nodey);
    Point p1y(nodey,7778);
    cout << "SizeNeigh: " << m_cloud->getPointsInSphericalNeighbourhood(p1y, m_search_r).size() << std::endl;
    return 0;
    */
    /*
    if (!omega::SystemManager::instance()->isMaster()) {
        return 0;
    }

    if (tracerstatus==TRACING) {
           debugSegment();
    }
    else if (m_waypoints.size()==0 && tracerstatus == DONOTHING)
    {
        */
        cout << "adding 2 waypoints " << endl;
        NodeGeometry* node;
        m_cloud->tryGetNode("r4",node);
        Point p1(node,7778);
        Point p2(node,7719);
        Point p3(node,1733);
        Point p4(node,1722);
        p1.index.node->getPointData(p1);
        p2.index.node->getPointData(p2);
        p3.index.node->getPointData(p3);
        p4.index.node->getPointData(p4);
        //float d=Utils::distance(p3.position,p4.position);
        //float d2=DIST3(p3.position,p4.position);
        if (playerID==1) {
            this->insertWaypoint(p3);
            this->insertWaypoint(p4);
        }
        else {
            this->insertWaypoint(p1);
            this->insertWaypoint(p2);
        }

        //setTracerStatus(START);
        //debugSegment();
    //}
    return 0;
}

# if 0
int FractureTracer::insert2pointsandtrace() {
    if (!omega::SystemManager::instance()->isMaster()) {
        return 0;
    }

    if (tracerstatus==TRACING) {
           debugSegment();
    }
    else if (m_waypoints.size()==0 && tracerstatus == DONOTHING)
    {
        cout << "adding 2 waypoints " << endl;
        NodeGeometry* node;
        m_cloud->tryGetNode("r4",node);
        Point p1(node,7778);
        Point p2(node,7719);
        Point p3(node,1733);
        Point p4(node,1722);
        p1.index.node->getPointData(p1);
        p2.index.node->getPointData(p2);
        this->insertWaypoint(p1);
        this->insertWaypoint(p2);
        setTracerStatus(START);
        debugSegment();
    }
    return 0;
}
#endif

# if 0
int FractureTracer::getSegmentCost(int p1, int p2, float search_r)
{
	int cost=1; //n.b. default value is 1 so that if no cost functions are used, the function doesn't crash (and returns the unweighted shortest path)
	if (m_cloud->hasColors()) //check cloud has colour data
	{
		if (COST_MODE & MODE::RGB)
			cost += getSegmentCostRGB(p1, p2);
		if (COST_MODE & MODE::DARK)
			cost += getSegmentCostDark(p1, p2);
		if (COST_MODE & MODE::LIGHT)
			cost += getSegmentCostLight(p1, p2);
		if (COST_MODE & MODE::GRADIENT)
			cost += getSegmentCostGrad(p1, p2, search_r);
	}
	if (m_cloud->hasDisplayedScalarField()) //check cloud has scalar field data
	{
		if (COST_MODE & MODE::SCALAR)
			cost += getSegmentCostScalar(p1, p2);
		if (COST_MODE & MODE::INV_SCALAR)
			cost += getSegmentCostScalarInv(p1, p2);
	}

	//these cost functions can be used regardless
	if (COST_MODE & MODE::CURVE)
		cost += getSegmentCostCurve(p1, p2);
	if (COST_MODE & MODE::DISTANCE)
		cost += getSegmentCostDist(p1, p2);

	return cost;
}



int FractureTracer::getSegmentCostDark(int p1, int p2)
{
	//return magnitude of the point p2
	//const ColorCompType* p1_rgb = m_cloud->getPointColor(p1);
	const ColorCompType* p2_rgb = m_cloud->getPointColor(p2);

	//return "taxi-cab" length
	return (p2_rgb[0] + p2_rgb[1] + p2_rgb[2]); //note: this will naturally give a maximum of 765 (=255 + 255 + 255)

	//return length
	//return sqrt((p2_rgb[0] * p2_rgb[0]) +
	//	(p2_rgb[1] * p2_rgb[1]) +
	//	(p2_rgb[2] * p2_rgb[2]))*2; //*2 makes it add to 884 for, which is a prereq for cost function mixing
}

int FractureTracer::getSegmentCostLight(int p1, int p2)
{
	//return the opposite of getCostDark
	return 765 - getSegmentCostDark(p1, p2);
}

int FractureTracer::getSegmentCostCurve(int p1, int p2)
{
	//put neighbourhood in a CCLib::Neighbourhood structure
	if (m_neighbours.size() > 4) //need at least 4 points to calculate curvature....
	{
		m_neighbours.push_back(m_p); //add center point onto end of neighbourhood

		//compute curvature
		CCLib::DgmOctreeReferenceCloud nCloud(&m_neighbours, static_cast<unsigned>(m_neighbours.size()));
		CCLib::Neighbourhood Z(&nCloud);
		float c = Z.computeCurvature(0, CCLib::Neighbourhood::CC_CURVATURE_TYPE::MEAN_CURV);
		
		m_neighbours.erase(m_neighbours.end()-1); //remove center point from neighbourhood (so as not to screw up loops)

		//curvature tends to range between 0 (high cost) and 10 (low cost), though it can be greater than 10 in extreme cases
		//hence we need to map to domain 0 - 10 and then transform that to the (integer) domain 0 - 884 to meet the cost function spec
		if (c > 10)
			c = 10;

		//scale curvature to range 0, 765
		c *= 76.5;

		//note that high curvature = low cost, hence subtract curvature from 765
		return 765 - c;
		
	}
	return 765; //unknown curvature - this point is high cost.
}

int FractureTracer::getSegmentCostGrad(int p1, int p2, float search_r)
{
	CCVector3 p = *m_cloud->getPoint(p2);
	const ColorCompType* p2_rgb = m_cloud->getPointColor(p2);
	int p_value = p2_rgb[0] + p2_rgb[1] + p2_rgb[2];

	if (m_neighbours.size() > 2) //need at least 2 points to calculate gradient....
	{
		//N.B. The following code is mostly stolen from the computeGradient function in CloudCompare
		CCVector3d sum(0, 0, 0);
		CCLib::DgmOctree::PointDescriptor n;
		for (int i = 0; i < m_neighbours.size(); i++)
		{
			n = m_neighbours[i];

			//vector from p1 to m_p
			CCVector3 deltaPos = *n.point - p;
			double norm2 = deltaPos.norm2d();

			//colour
			const ColorCompType* c = m_cloud->getPointColor(n.pointIndex);
			int c_value = c[0] + c[1] + c[2];

			//calculate gradient weighted by distance to the point (i.e. divide by distance^2)
			if (norm2 > ZERO_TOLERANCE)
			{
				//color magnitude difference
				int deltaValue = p_value - c_value;
				//divide by norm^2 to get distance-weighted gradient
				deltaValue /= norm2; //we divide by 'norm' to get the normalized direction, and by 'norm' again to get the gradient (hence we use the squared norm)
				//sum stuff
				sum.x += deltaPos.x * deltaValue; //warning: here 'deltaValue'= deltaValue / squaredNorm(deltaPos) ;)
				sum.y += deltaPos.y * deltaValue;
				sum.z += deltaPos.z * deltaValue;
			}
		}

		float gradient = sum.norm() / m_neighbours.size();

		//ensure gradient is lass than a case-specific maximum gradient (colour change from white to black across a distance or search_r,
		//                                                                                  giving a gradient of (255+255+255) / search_r)
		gradient = std::min(gradient, 765 / search_r);
		gradient *= search_r; //scale between 0 and 765
		return 765 - gradient; //return inverse gradient (high gradient = low cost)
	}
	return 765; //no gradient = high cost
}



int FractureTracer::getSegmentCostScalar(int p1, int p2)
{
    //m_cloud->getCurrentDisplayedScalarFieldIndex();
	ccScalarField* sf = static_cast<ccScalarField*>(m_cloud->getCurrentDisplayedScalarField());
	return (sf->getValue(p2)-sf->getMin()) * (765 / (sf->getMax()-sf->getMin())); //return scalar field value mapped to range 0 - 765
}

int FractureTracer::getSegmentCostScalarInv(int p1, int p2)
{
	ccScalarField* sf = static_cast<ccScalarField*>(m_cloud->getCurrentDisplayedScalarField());
	return (sf->getMax() - sf->getValue(p2)) * (765 / (sf->getMax() - sf->getMin())); //return inverted scalar field value mapped to range 0 - 765
}
#endif
#if TODO
ccPlane* FractureTracer::fitPlane(int surface_effect_tolerance, float min_planarity)
{
	//put all "trace" points into the cloud
	finalizePath();
	
	if (size() < 3)
		return 0; //need three points to fit a plane

	//check we are not trying to fit a plane to a line
	CCLib::Neighbourhood Z(this);

	//calculate eigenvalues of neighbourhood
	CCLib::SquareMatrixd cov = Z.computeCovarianceMatrix();
	CCLib::SquareMatrixd eigVectors; std::vector<double> eigValues;
	if (Jacobi<double>::ComputeEigenValuesAndVectors(cov, eigVectors, eigValues, true))
	{
		//sort eigenvalues into decending order
		std::sort(eigValues.rbegin(), eigValues.rend());

		float y = eigValues[1]; //middle eigen
		float z = eigValues[2]; //smallest eigen (parallel to plane normal)

		//calculate planarity (0 = line or random, 1 = plane)
		float planarity = 1.0f - z / y;
		if (planarity < min_planarity)
		{
			return 0;
		}
	}

	//fit plane
	double rms = 0.0; //output for rms
	ccPlane* p = ccPlane::Fit(this, &rms);
    
	//calculate and store plane attributes
	//get plane normal vector
	CCVector3 N(p->getNormal());
	//We always consider the normal with a positive 'Z' by default!
	if (N.z < 0.0)
		N *= -1.0;

	//calculate dip/dip direction
	float strike, dip;
	ccNormalVectors::ConvertNormalToStrikeAndDip(N, strike, dip);
	//QString dipAndDipDirStr = ccNormalVectors::ConvertStrikeAndDipToString(s, d);
	QString dipAndDipDirStr = QString("%1/%2").arg((int)strike, 3, 10, QChar('0')).arg((int)dip, 2, 10, QChar('0'));
	p->setName(dipAndDipDirStr);
	//calculate centroid
	CCVector3 C = p->getCenter();

	//store attributes (centroid, strike, dip, RMS) on plane
	QVariantMap* map = new QVariantMap();
	map->insert("Cx", C.x); map->insert("Cy", C.y); map->insert("Cz", C.z); //centroid
	map->insert("Nx", N.x); map->insert("Ny", N.y); map->insert("Nz", N.z); //normal
	map->insert("Strike", strike); map->insert("Dip", dip); //strike & dip
	map->insert("RMS", rms); //rms
	map->insert("Radius", m_search_r); //search radius
	p->setMetaData(*map, true);

	//test for 'surface effect'
	if (m_cloud->hasNormals())
	{
		//calculate average normal of points on trace
		CCVector3 n_avg;
		for (unsigned i = 0; i < size(); i++)
		{
			//get normal vector
			CCVector3 n = ccNormalVectors::GetNormal(m_cloud->getPointNormalIndex(this->getPointGlobalIndex(i)));
			n_avg += n;
		}
		n_avg *= (PC_ONE / size()); //turn sum into average


		//compare to plane normal
		CCVector3 n = p->getNormal();
		if (acos(n_avg.dot(n)) < 0.01745329251*surface_effect_tolerance) //0.01745329251 converts deg to rad
		{
			//this is a false (surface) plane - reject
			return 0; //don't return plane
		}
	}

	//all is good! Return the plane :)
	return p;
}
#endif

/* replaced by pcinfo spacing devided by hierarchy level
float FractureTracer::calculateOptimumSearchRadius()
{
#if TODO
	CCLib::DgmOctree::NeighboursSet neighbours;

	//setup octree & values for nearest neighbour searches
	ccOctree::Shared oct = m_cloud->getOctree();
	if (!oct)
	{
		oct = m_cloud->computeOctree(); //if the user clicked "no" when asked to compute the octree then tough....
	}

	//init vars needed for nearest neighbour search
	unsigned char level = oct->findBestLevelForAGivenPopulationPerCell(2);
	CCLib::ReferenceCloud* nCloud = new  CCLib::ReferenceCloud(m_cloud);

	//pick 15 random points
	int r;
	unsigned int npoints = m_cloud->size();
	double d,dsum=0;
	srand(npoints); //set seed as n for repeatability
	for (unsigned int i = 0; i < 30; i++)
	{
		//int rn = rand() * rand(); //need to make bigger than rand max...
		r = (rand()*rand()) % npoints; //random(ish) number between 0 and n

		//find nearest neighbour for point
		nCloud->clear(false);
		int n = oct->findPointNeighbourhood(m_cloud->getPoint(r), nCloud, 2, level, d);

		if (d != -1) //if a point was found
		{
			dsum += sqrt(d);
		}
	}
	
	//average nearest-neighbour distances
	d = dsum / 30;
	
	//return a number slightly larger than the average distance
	return d*1.5;
#endif
    return 0;
}
*/

//static QSharedPointer<ccSphere> c_unitPointMarker(0);

void FractureTracer::render()
{
    if (tracerstatus==TRACING_SEGMENTS)
    {
        checkOnTraces();
    }

    if (m_waypoints.size() == 0)
        return;
    Point p;
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

    glEnable(GL_PROGRAM_POINT_SIZE_EXT);
    glPointSize(pointscale);
    glColor3f(1.0, 1.0, 0.0); //yellow
    glBegin(GL_POINTS);

    for (std::vector < Point > ::iterator waypoint = m_waypoints.begin() ; waypoint != m_waypoints.end(); ++waypoint)
    {
        p=(*waypoint);
        //PointIndex pi = p.index;
        //p=m_cloud->getPointFromIndex(pi);
        glVertex3f(p.position[0],p.position[1],p.position[2]);
    }
    glEnd();
    /*
    if (tracerstatus==TRACING_SEGMENT) {
        glPointSize(pointscale/4);
        glColor3f(0.8, 0.8, 0.8); //white
        glBegin(GL_POINTS);
        for (std::vector < Point > ::iterator it = m_neighbours.begin() ; it != m_neighbours.end(); ++it)
        {
            p=(*it);
            glVertex3f(p.position[0],p.position[1],p.position[2]);
        }
        glEnd();
    }*/
/*
    if (tracerstatus==TRACING_SEGMENTS) { //debugging of openset and current
        glPointSize(5);
        glColor3f(1.0, 0.0, 0.0); //red
        glBegin(GL_POINTS);

        for (std::map<PointIndex, PointIndex>::iterator it=m_openSet.begin();it!=m_openSet.end();++it)
        {
            p=m_cloud->getPointFromIndex((*it).first);
            glVertex3f(p.position[0],p.position[1],p.position[2]);
        }
        glEnd();    

        glPointSize(pointscale*2);
        glColor3f(0.0, 1.0, 0.0); //green
        glBegin(GL_POINTS);
        glVertex3f(m_current.position[0],m_current.position[1],m_current.position[2]);
        glEnd();
    }
*/

    if (tracerstatus==FINISHED_DONE) {
        glPointSize(pointscale*2);
        glColor3f(1.0, 1.0, 1.0); //white
        glLineWidth(4.0);
        glBegin(GL_LINES);
        //glBegin(GL_POINTS);

        std::vector< TracingSegment* >::iterator segit= m_trace.begin();
        TracingSegment * seg;
        std::deque < Point > segpath;
        std::deque < Point >::iterator ptsit;
        for (;segit!=m_trace.end();segit++)
        {
            seg=(*segit);
            segpath =seg->getSegmentPathRef();
            for (ptsit = segpath.begin() ; ptsit != segpath.end(); ++ptsit)
            {
                p=(*ptsit);
                glVertex3f(p.position[0],p.position[1],p.position[2]);
            }
        }
        glEnd();
    }

    //glLineWidth(4.0);
    //glColor3f(m_tracers[playerid]->rayColor[0], m_tracers[playerid]->rayColor[1], m_tracers[playerid]->rayColor[2]);
    //glBegin(GL_LINES);
    //glVertex3f(spos[0], spos[1], spos[2]);
    //glVertex3f(epos[0], epos[1], epos[2]);
    //glEnd();


}

/*
void FractureTracer::drawMeOnly(CC_DRAW_CONTEXT& context)
{
	if (!MACRO_Foreground(context)) //2D foreground only
		return; //do nothing

	if (MACRO_Draw3D(context))
	{
		if (m_waypoints.size() == 0) //no points -> bail!
			return;

		//get the set of OpenGL functions (version 2.1)
		QOpenGLFunctions_2_1 *glFunc = context.glFunctions<QOpenGLFunctions_2_1>();
		if (glFunc == nullptr) {
			assert(false);
			return;
		}

		//check sphere exists
		if (!c_unitPointMarker)
		{
			c_unitPointMarker = QSharedPointer<ccSphere>(new ccSphere(1.0f, 0, "PointMarker", 6));
			
			c_unitPointMarker->showColors(true);
			c_unitPointMarker->setVisible(true);
			c_unitPointMarker->setEnabled(true);
		}

		//not sure what this does, but it looks like fun
		CC_DRAW_CONTEXT markerContext = context; //build-up point maker own 'context'
		markerContext.drawingFlags &= (~CC_DRAW_ENTITY_NAMES); //we must remove the 'push name flag' so that the sphere doesn't push its own!
		markerContext.display = 0;

		//get camera info
		ccGLCameraParameters camera;
		glFunc->glGetIntegerv(GL_VIEWPORT, camera.viewport);
		glFunc->glGetDoublev(GL_PROJECTION_MATRIX, camera.projectionMat.data());
		glFunc->glGetDoublev(GL_MODELVIEW_MATRIX, camera.modelViewMat.data());

		//set draw colour
        c_unitPointMarker->setTempColor(omega::Color::Green);
		
		//draw key-points
		const ccViewportParameters& viewportParams = context.display->getViewportParameters();
		for (unsigned i = 0; i < m_waypoints.size(); i++)
		{
			glFunc->glMatrixMode(GL_MODELVIEW);
			glFunc->glPushMatrix();

			const CCVector3* P = m_cloud->getPoint(m_waypoints[i]);
			ccGL::Translate(glFunc, P->x, P->y, P->z);
			float scale = context.labelMarkerSize * m_relMarkerScale * 0.15;
			if (viewportParams.perspectiveView && viewportParams.zFar > 0)
			{
				//in perspective view, the actual scale depends on the distance to the camera!
				const double* M = camera.modelViewMat.data();
				double d = (camera.modelViewMat * CCVector3d::fromArray(P->u)).norm();
				double unitD = viewportParams.zFar / 2; //we consider that the 'standard' scale is at half the depth
				scale = static_cast<float>(scale * sqrt(d / unitD)); //sqrt = empirical (probably because the marker size is already partly compensated by ccGLWindow::computeActualPixelSize())
			}
			glFunc->glScalef(scale, scale, scale);
			c_unitPointMarker->draw(markerContext);
			glFunc->glPopMatrix();
		}

		//set draw colour
        c_unitPointMarker->setTempColor(omega::Color::Yellow);

		//draw trace points
		for (std::deque<int> seg : m_trace)
		{
			for (int p : seg)
			{
				glFunc->glMatrixMode(GL_MODELVIEW);
				glFunc->glPushMatrix();

				const CCVector3* P = m_cloud->getPoint(p);
				ccGL::Translate(glFunc, P->x, P->y, P->z);
				float scale = context.labelMarkerSize * m_relMarkerScale * 0.1;
				if (viewportParams.perspectiveView && viewportParams.zFar > 0)
				{
					//in perspective view, the actual scale depends on the distance to the camera!
					const double* M = camera.modelViewMat.data();
					double d = (camera.modelViewMat * CCVector3d::fromArray(P->u)).norm();
					double unitD = viewportParams.zFar / 2; //we consider that the 'standard' scale is at half the depth
					scale = static_cast<float>(scale * sqrt(d / unitD)); //sqrt = empirical (probably because the marker size is already partly compensated by ccGLWindow::computeActualPixelSize())
				}
				glFunc->glScalef(scale, scale, scale);
				c_unitPointMarker->draw(markerContext);
				glFunc->glPopMatrix();
			}
		}

		//draw lines
		for (std::deque<int> seg : m_trace)
		{
			glFunc->glBegin(GL_LINE_STRIP);
			for (int p : seg)
			{
				ccGL::Vertex3v(glFunc, m_cloud->getPoint(p)->u);
			}
			glFunc->glEnd();
		}
	}
}
*/

void FractureTracer::setComponentColor(std::string component,float r,float g,float b){

    if (component == "ray") {rayColor[0]=r;rayColor[1]=g;rayColor[2]=b;}
    else if (component=="selection"){selectionColor[0]=r;selectionColor[1]=g;selectionColor[2]=b;}
    else if (component=="trace"){traceColor[0]=r;traceColor[1]=g;traceColor[2]=b;}
    else if (component=="waypoint"){waypointColor[0]=r;waypointColor[1]=g;waypointColor[2]=b;}
    else{cout << "cannot set color for tracercomponent with name: "<< component <<endl;return;}
}

}; // namespace gigapoint
