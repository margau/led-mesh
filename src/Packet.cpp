#include "Packet.hpp"

Packet::Packet(Route _r, RgbColor _c) : route(_r), color(_c) {
    position = 0;
}