#pragma once
#include "glwrapper.hpp"
#include "chunk_handler.hpp"
#include "glwrapper.hpp"
#include <unordered_map>
#include "inttup.hpp"

#ifndef LOAD_WIDTH
#define LOAD_WIDTH 8
#endif

class ChunkFormation {
public:
    IntTup positions[LOAD_WIDTH*LOAD_WIDTH];

    Chunk chunks[LOAD_WIDTH*LOAD_WIDTH];
    //entt::entity entities[LOAD_WIDTH*LOAD_WIDTH];
    ChunkFormation(GLWrapper& wr, std::unordered_map<IntTup, int, IntTupHash> &worldmap, entt::registry& reg, std::unordered_map<IntTup, float, IntTupHash> &heightmap);
    void set_position(glm::vec3& pos);
private:
    GLWrapper& m_wrap;
    std::unordered_map<IntTup, int, IntTupHash>& m_world;
    std::unordered_map<IntTup, float, IntTupHash>& m_height;
    entt::registry& m_reg;
};

#ifdef CHUNKFORMATION_IMP

void ChunkFormation::set_position(glm::vec3& camera_pos) {

    glm::vec3 chunk_f_pos = camera_pos / (float)CHUNK_WIDTH;
    //std::cout << "chunk F pos: " << chunk_f_pos.x  << " " << chunk_f_pos.z << std::endl;
    static bool initial_gen = true;
    if(initial_gen)
    {
        for(int i = 0; i < LOAD_WIDTH * LOAD_WIDTH; ++i)
        {
            IntTup new_pos = positions[i];
            chunks[i].move_to(new_pos);
            chunks[i].rebuild();
        }
        initial_gen = false;
    }

    static IntTup last_pos(0,0);

    IntTup chunk_i_pos(
        static_cast<int>(std::round(chunk_f_pos.x)),
        0,
        static_cast<int>(std::round(chunk_f_pos.z)));
    //std::cout << "chunk I pos: " << chunk_i_pos.x << " " << chunk_i_pos.z << std::endl;

    static std::vector<IntTup> positions_loaded;

    if(chunk_i_pos != last_pos)
    {
        //std::cout << "Movement: " << chunk_i_pos.x - last_pos.x << " " << chunk_i_pos.z - last_pos.z <<  std::endl;
        IntTup movement(chunk_i_pos.x - last_pos.x, chunk_i_pos.z - last_pos.z);


        last_pos = chunk_i_pos;

        for(int i = 0; i < LOAD_WIDTH * LOAD_WIDTH; ++i)
        {
            IntTup new_pos = positions[i] + chunk_i_pos;
            chunks[i].move_to(new_pos);
            chunks[i].rebuild();
        }
    }

}

ChunkFormation::ChunkFormation(GLWrapper& wr, std::unordered_map<IntTup, int, IntTupHash> &worldmap, entt::registry& reg, std::unordered_map<IntTup, float, IntTupHash> &heightmap)
    : m_wrap(wr), m_world(worldmap), m_reg(reg), m_height(heightmap)
{

    int half = LOAD_WIDTH / 2;

    int ind = 0;
    for(int i = -half; i < half; ++i)
    {
        for(int k = -half; k < half; ++k)
        {
            this->positions[ind] = IntTup(i, k);

            this->chunks[ind] = Chunk(glm::vec2(i, k), reg, wr, worldmap, heightmap);
            ind++;
        }
    }


}

#endif