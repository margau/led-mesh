#include "Segment.hpp"

Segment::Segment(uint16_t _length, RgbColor _background, uint16_t _offset) {
    offset = _offset;
    length = _length;
    background = _background;
    RgbColor pixels[length];
}