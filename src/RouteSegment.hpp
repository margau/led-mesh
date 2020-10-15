#ifndef ROUTESEGMENT_HPP
#define ROUTESEGMENT_HPP
#include "Segment.hpp"
class RouteSegment
{
public:
    RouteSegment(Segment _segment, bool _reverse=false);
    Segment segment;
    bool reverse;
};

#endif