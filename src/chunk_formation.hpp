#pragma once
#include "glwrapper.hpp"
#include "chunk_handler.hpp"
#include "glwrapper.hpp"
#include <unordered_map>
#include "inttup.hpp"

#ifndef LOAD_WIDTH
#define LOAD_WIDTH 16
#endif

#define FULL_SIZE (LOAD_WIDTH+1)*(LOAD_WIDTH+1)

class ChunkFormation {
public:
    IntTup positions[FULL_SIZE];

    Chunk chunks[FULL_SIZE];
    //entt::entity entities[LOAD_WIDTH*LOAD_WIDTH];
    ChunkFormation(GLWrapper& wr, std::unordered_map<IntTup, int, IntTupHash> &worldmap, entt::registry& reg, std::unordered_map<IntTup, unsigned char, IntTupHash> &heightmap);
    void set_position(glm::vec3& pos);
    void reload(glm::vec3& pos);
private:
    GLWrapper& m_wrap;
    std::unordered_map<IntTup, int, IntTupHash>& m_world;
    std::unordered_map<IntTup, unsigned char, IntTupHash>& m_height;
    entt::registry& m_reg;
};

#ifdef CHUNKFORMATION_IMP

void ChunkFormation::reload(glm::vec3& camera_pos) {
    glm::vec3 chunk_f_pos = camera_pos / (float)CHUNK_WIDTH;
    IntTup chunk_i_pos(
    static_cast<int>(std::round(chunk_f_pos.x)),
    0,
    static_cast<int>(std::round(chunk_f_pos.z))); 


 IntTup adjusted_chunk_i_pos = chunk_i_pos + IntTup(static_cast<int>(m_wrap.cameraDirection.x * 4), 0, static_cast<int>(m_wrap.cameraDirection.z * 4));

        std::sort(this->chunks, this->chunks + FULL_SIZE, [adjusted_chunk_i_pos](Chunk& a, Chunk& b) {
                int dista = (a.chunk_position.x - adjusted_chunk_i_pos.x) + (a.chunk_position.y - adjusted_chunk_i_pos.z);
                int distb = (b.chunk_position.x - adjusted_chunk_i_pos.x) + (b.chunk_position.y - adjusted_chunk_i_pos.z);
                return std::abs(dista) > std::abs(distb);
        });


    int index = 0;
    for(int i = 0; i < FULL_SIZE; ++i) 
    {
        
        IntTup new_pos = positions[i] + chunk_i_pos;
        if(Chunk::donespots.find(new_pos) == Chunk::donespots.end()) {
            chunks[index].move_to(new_pos);
            chunks[index].rebuild();
            index++;
        }

    }
}

void ChunkFormation::set_position(glm::vec3& camera_pos) {

    glm::vec3 chunk_f_pos = camera_pos / (float)CHUNK_WIDTH; //your "float chunk position", i.e. 1.345
    //std::cout << "chunk F pos: " << chunk_f_pos.x  << " " << chunk_f_pos.z << std::endl;
    IntTup chunk_i_pos(
        static_cast<int>(std::round(chunk_f_pos.x)),
        0,
        static_cast<int>(std::round(chunk_f_pos.z))); 

    static bool initial_gen = true;
    if(initial_gen)
    {
        for(int i = 0; i < FULL_SIZE; ++i)
        {
            IntTup new_pos = positions[i] + chunk_i_pos;
            chunks[i].move_to(new_pos);
            chunks[i].rebuild();
        }

        initial_gen = false;
    }

    static IntTup last_pos(0,0);
       
    //std::cout << "chunk I pos: " << chunk_i_pos.x << " " << chunk_i_pos.z << std::endl;

    if(chunk_i_pos != last_pos)
    {
        // std::cout << "Movement: " << chunk_i_pos.x - last_pos.x << " " << chunk_i_pos.z - last_pos.z <<  std::endl;
        IntTup movement(chunk_i_pos.x - last_pos.x, chunk_i_pos.z - last_pos.z); //HOW different it is.
        IntTup adjusted_chunk_i_pos = chunk_i_pos + IntTup(static_cast<int>(m_wrap.cameraDirection.x * 4), 0, static_cast<int>(m_wrap.cameraDirection.z * 4));

        std::sort(this->chunks, this->chunks + FULL_SIZE, [adjusted_chunk_i_pos](Chunk& a, Chunk& b) {
                int dista = (a.chunk_position.x - adjusted_chunk_i_pos.x) + (a.chunk_position.y - adjusted_chunk_i_pos.z);
                int distb = (b.chunk_position.x - adjusted_chunk_i_pos.x) + (b.chunk_position.y - adjusted_chunk_i_pos.z);
                return std::abs(dista) > std::abs(distb);
        });


        if(movement.x != 0)  {



            int start = std::max(0, (FULL_SIZE - (LOAD_WIDTH+1)) * movement.x);
            int index = 0;
            for(int t = start; t < start + LOAD_WIDTH+1; t += 1)
            {
                IntTup newpos = positions[t] + chunk_i_pos;
                if(Chunk::donespots.find(newpos) == Chunk::donespots.end())
                {
                    this->chunks[index].move_to(newpos);
                    this->chunks[index].rebuild();
                    index++;
                }
            }
        }

        if(movement.z != 0) {

 
            int start = std::max(0, ((LOAD_WIDTH)*movement.z));
            int index = 0;
            for(int t = start; t < FULL_SIZE; t += LOAD_WIDTH+1)
            {
                IntTup newpos = positions[t] + chunk_i_pos;
                if(Chunk::donespots.find(newpos) == Chunk::donespots.end())
                {
                    this->chunks[index].move_to(newpos);
                    this->chunks[index].rebuild();
                    index++;
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
            this->positions[ind] = IntTup(i, k);

            ind++;
        }
    }
    for(int i = 0; i < FULL_SIZE; ++i) {
        this->chunks[i] = Chunk(glm::vec2(this->positions[i].x, this->positions[i].z), reg, wr, worldmap, heightmap);
    }


}

#endif
