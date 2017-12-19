#ifndef __TRACING_SEGMENT
#define __TRACING_SEGMENT

#include "Utils.h"
#include <deque>


namespace gigapoint {

class PointCloud;
class FractureTracer;

class TracingSegment
{
public:

    enum SEGMENTSTATUS
    {
        SEGMENT_INQUEUE =1,
        SEGMENT_START = 2,
        SEGMENT_TRACING = 3,
        SEGMENT_FINISHED_FOUND = 4,
        SEGMENT_FINISHED_NOTFOUND = 5
    };

    TracingSegment(FractureTracer*,Point,Point);
    std::deque<Point> & getSegmentPathRef() {return segmentPath;}
    void trace();
    void setStatus(SEGMENTSTATUS stat){status=stat;}

    SEGMENTSTATUS getStatus(){return status;}

private:
    FractureTracer* tracer;
    SEGMENTSTATUS status;
    std::deque<Point> segmentPath;
    Point start,end;
    int iter_count;
    static const int MAX_COST=99999;

    typedef struct PointData_t {
        bool visited;
        int cost;
        PointIndex previous;
        PointData_t() : cost(-1),visited(false)
        {

        }
        PointData_t(bool b) : cost(-1),visited(b)
        {

        }
    } PointData;

    int getSegmentCostRGB(Point p1_rgb, Point p2_rgb);
    Point m_start_rgb;
    Point m_end_rgb; //[r,g,b] values for start and end nodes

};

}; // namespave gigapoint
#endif //TRACING_SEGMENT
