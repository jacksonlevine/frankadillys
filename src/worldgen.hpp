#pragma once
#include "inttup.hpp"
#include <unordered_map>
#include "perlin.h"
#include <string>
void generate_world(std::unordered_map<IntTup, int, IntTupHash>& worldmap, std::unordered_map<IntTup, float, IntTupHash>& heightmap);

#ifdef WORLDGEN_IMP

perlin p;
float noise_func(int x, int y, int z)
{
    return 20.0f + p.noise(((float)x)/15.35f, ((float)y)/15.35f, ((float)z)/15.35f) * 10.0f;
}
void generate_world(std::unordered_map<IntTup, int, IntTupHash>& worldmap, std::unordered_map<IntTup, float, IntTupHash>& heightmap)
{
    for(int i = -100; i < 100; i++)
    {

        for(int k = -100; k < 100; k++)
        {

            heightmap.insert_or_assign(IntTup(i,k), std::trunc(35.0f + p.noise(((float)i)/105.35f, ((float)34.23)/105.35f, ((float)k)/105.35f)*5.0f));
            //std::cout << std::to_string( noise_func(i/103.4f,35.2120,k/102.0f)*10) << " " << std::endl;

            for(int j = 0; j < CHUNK_HEIGHT; j++)
            {
                int density = std::max(0.0f, noise_func(i,j,k) - std::max(((float)j/3.0f), 0.0f));
                if(density > 10)
                {
                    worldmap.insert_or_assign(IntTup(i, j, k), 0);
                }
            }
        }
    }
}
#endif