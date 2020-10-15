#ifndef ROUTE_HPP
#define ROUTE_HPP
#include "RouteSegment.hpp"
#include <vector>
class Route
{
public:
    Route(std::vector<RouteSegment> _segments);
    std::vector<RouteSegment> segments;
    uint16_t length;
};

#endif