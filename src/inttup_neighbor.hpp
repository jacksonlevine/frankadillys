#pragma once
#include "inttup.hpp"
#include "cube_helper.hpp"
class Neighbor {
public:
    CubeFace face;
    IntTup tup;
    Neighbor(CubeFace f, IntTup t);
};

std::vector<Neighbor> get_neighbors(IntTup tup);

#ifdef NEIGHBORS_IMPLEMENTATION
Neighbor::Neighbor(CubeFace f, IntTup t) : face(f), tup(t) {};
std::vector<Neighbor> get_neighbors(IntTup t) {
    std::vector<Neighbor> result =
    {
        Neighbor(TOP, t + IntTup(0, 1, 0)),
        Neighbor(BOTTOM, t + IntTup(0, -1, 0)),
        Neighbor(LEFT, t + IntTup(-1, 0, 0)),
        Neighbor(RIGHT, t + IntTup(1, 0, 0)),
        Neighbor(FRONT, t + IntTup(0, 0, 1)),
        Neighbor(BACK, t + IntTup(0, 0, -1))
    };
    return result;
}
#endif