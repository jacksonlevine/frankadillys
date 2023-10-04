#pragma once
#include "perlin.h"
#include "inttup.hpp"

bool block_noise_func(int x, int y, int z);
bool block_noise_func(IntTup& tup);

unsigned char height_noise_func(int x, int z);
unsigned char height_noise_func(IntTup& tup);

#ifdef PERLINSTUFF_IMP

perlin p;

bool block_noise_func(int x, int y, int z)
{
    float t = 20.0f + p.noise(((float)x)/15.35f, ((float)y)/15.35f, ((float)z)/15.35f) * 10.0f;
    int density = std::max(0.0f, t - std::max(((float)y/3.0f), 0.0f));
    return density > 10;
}

bool block_noise_func(IntTup& tup)
{
    return block_noise_func(tup.x, tup.y, tup.z);
}

unsigned char height_noise_func(int x, int z)
{
    return static_cast<unsigned char>(std::trunc(35.0f + p.noise(((float)x)/105.35f, (float)34.23f, ((float)z)/105.35f)*10.0f));
}

unsigned char height_noise_func(IntTup& tup)
{
    return height_noise_func(tup.x, tup.z);
}


#endif