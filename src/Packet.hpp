#ifndef PACKET_HPP
#define PACKET_HPP
#include "Route.hpp"
#include "Packet.hpp"
#include <NeoPixelBus.h>
class Packet
{
public:
    Packet(Route _r, RgbColor _c);
    Route route;
    uint16_t position;
    RgbColor color;
};

#endif