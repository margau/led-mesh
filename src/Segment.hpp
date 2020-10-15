#ifndef SEGMENT_HPP
#define SEGMENT_HPP
#include <NeoPixelBus.h>
class Segment
{
public:
    Segment(uint16_t _length, RgbColor _background, uint16_t _offset);
    uint16_t offset;
    uint16_t length;
    RgbColor pixels[];
    RgbColor background;
};

#endif