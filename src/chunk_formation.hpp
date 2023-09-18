#pragma once
#include "glwrapper.hpp"
#include "chunk_handler.hpp"
#include "glwrapper.hpp"
#include <unordered_map>
#include "inttup.hpp"

#ifndef LOAD_WIDTH
#define LOAD_WIDTH 8
#endif

#define FULL_SIZE (LOAD_WIDTH+1)*(LOAD_WIDTH+1)

class ChunkFormation {
public:
    IntTup positions[FULL_SIZE];

    Chunk chunks[FULL_SIZE];
    //entt::entity entities[LOAD_WIDTH*LOAD_WIDTH];
    ChunkFormation(GLWrapper& wr, std::unordered_map<IntTup, int, IntTupHash> &worldmap, entt::registry& reg, std::unordered_map<IntTup, unsigned char, IntTupHash> &heightmap);
    void set_position(glm::vec3& pos);
private:
    GLWrapper& m_wrap;
    std::unordered_map<IntTup, int, IntTupHash>& m_world;
    std::unordered_map<IntTup, unsigned char, IntTupHash>& m_height;
    entt::registry& m_reg;
};

#ifdef CHUNKFORMATION_IMP

void ChunkFormation::set_position(glm::vec3& camera_pos) {

    glm::vec3 chunk_f_pos = camera_pos / (float)CHUNK_WIDTH; //your "float chunk position", i.e. 1.345 
    //std::cout << "chunk F pos: " << chunk_f_pos.x  << " " << chunk_f_pos.z << std::endl;


    static bool initial_gen = true;
    if(initial_gen) 
    {
        for(int i = 0; i < FULL_SIZE; ++i) //lay  out the chunks like a deck of cards 
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
        static_cast<int>(std::round(chunk_f_pos.z)));            //only change and update last_pos (STATIC) if your chunk_i_pos (integer chunk position) has changed
    //std::cout << "chunk I pos: " << chunk_i_pos.x << " " << chunk_i_pos.z << std::endl;

    if(chunk_i_pos != last_pos)
    {
        std::cout << "Movement: " << chunk_i_pos.x - last_pos.x << " " << chunk_i_pos.z - last_pos.z <<  std::endl;
        IntTup movement(chunk_i_pos.x - last_pos.x, chunk_i_pos.z - last_pos.z); //HOW different it is.





       
        std::vector<entt::entity> offeredupids;

        //ERASE THE BACK 
        if(movement.z != 0) {
            for(int t = std::max(0, LOAD_WIDTH * -movement.z); t < FULL_SIZE; t += LOAD_WIDTH+1) {
                if(std::find(offeredupids.begin(), offeredupids.end(), chunks[t].id) == offeredupids.end())
                {
                    offeredupids.push_back(chunks[t].id);
                }
                
            }
        }
        if(movement.x != 0) {
            for(int t = std::max(0, LOAD_WIDTH * -movement.x) * (LOAD_WIDTH+1); t < FULL_SIZE; t += 1) {
                if(std::find(offeredupids.begin(), offeredupids.end(), chunks[t].id) == offeredupids.end())
                {
                    offeredupids.push_back(chunks[t].id);
                }
                
            }
        }


        //SHIFT THE MIDDLES
        for(int i = 0; i < FULL_SIZE; ++i)
        {
            int newIndex = i - (movement.x * LOAD_WIDTH) - movement.z;
            if(newIndex > 0 && newIndex < FULL_SIZE) {
                chunks[newIndex].newid = chunks[i].id;
                chunks[newIndex].chunk_position = chunks[i].old_chunk_position;
            }
        }

        //REBUILD THE FRONT
        if(movement.z != 0) {
            for(int t = std::max(0, LOAD_WIDTH * movement.z); t < FULL_SIZE; t += LOAD_WIDTH+1) {
                if(offeredupids.size() > 0)
                {
                    chunks[t].newid = offeredupids.back();
                    offeredupids.pop_back();

                    IntTup new_pos = positions[t] + chunk_i_pos;
                    chunks[t].move_to(new_pos);
                    chunks[t].rebuild();
                }
                
            }
        }
        if(movement.x != 0) {
            for(int t = std::max(0, LOAD_WIDTH * movement.x) * (LOAD_WIDTH+1); t < FULL_SIZE; t += 1) {
                if(offeredupids.size() > 0)
                {
                    chunks[t].newid = offeredupids.back();
                    offeredupids.pop_back();

                    IntTup new_pos = positions[t] + chunk_i_pos;
                    chunks[t].move_to(new_pos);
                    chunks[t].rebuild();
                }
                
            }
        }



    

         last_pos = chunk_i_pos;
    }

}

ChunkFormation::ChunkFormation(GLWrapper& wr, std::unordered_map<IntTup, int, IntTupHash> &worldmap, entt::registry& reg, std::unordered_map<IntTup, unsigned char, IntTupHash> &heightmap)
    : m_wrap(wr), m_world(worldmap), m_reg(reg), m_height(heightmap)
{

    int half = LOAD_WIDTH / 2;

    int ind = 0;
    for(int i = -half; i < half+1; ++i)
    {
        for(int k = -half; k < half+1; ++k)
        {
            this->positions[ind] = IntTup(i, k); //
            
            this->chunks[ind] = Chunk(glm::vec2(i, k), reg, wr, worldmap, heightmap); //initialize chunk object for position
            ind++;
        }
    }


}

#endif