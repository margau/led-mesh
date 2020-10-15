#include "Route.hpp"

Route::Route(std::vector<RouteSegment> _segments) : segments(_segments) {
    length = 0;
    // Calulate pixel length for this route
    for(RouteSegment s: segments) {
        length+=s.segment.length;
    }
}