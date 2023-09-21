#pragma once
#include "glwrapper.hpp"
#include <entt/entt.hpp>

#include "cube_helper.hpp"
#include "mesh_component.hpp"


#include "inttup.hpp"
#include "inttup_neighbor.hpp"

#include <string>

#include "perlin_stuff.hpp"

#ifndef CHUNK_WIDTH
#define CHUNK_WIDTH 18
#endif

#ifndef CHUNK_HEIGHT
#define CHUNK_HEIGHT 64
#endif

class Chunk {

public:
    entt::entity id;
    entt::entity newid;
    glm::vec2 chunk_position;
    glm::vec2 old_chunk_position;
    Chunk();
    Chunk(glm::vec2 c_pos, entt::registry& r, GLWrapper& w, std::unordered_map<IntTup, int, IntTupHash>& wo, std::unordered_map<IntTup, unsigned char, IntTupHash>& hm);

    Chunk& operator=(const Chunk& other);
    Chunk& rebuild();
    void move_to(IntTup ipos);
    void setx(int x);
    void setz(int z);
    static std::unordered_map<IntTup, unsigned char, IntTupHash> donespots;
private:
    entt::registry* m_reg;
    GLWrapper* m_wrap;
    std::unordered_map<IntTup, int, IntTupHash>* m_world;
    std::unordered_map<IntTup, unsigned char, IntTupHash>* m_height;
};

#ifdef CHUNK_IMPLEMENTATION

std::unordered_map<IntTup, unsigned char, IntTupHash> Chunk::donespots;

Chunk::Chunk(glm::vec2 c_pos, entt::registry& r, GLWrapper& w, std::unordered_map<IntTup, int, IntTupHash>& wo, std::unordered_map<IntTup, unsigned char, IntTupHash>& hm)
    : m_reg(&r), m_wrap(&w), m_world(&wo), chunk_position(c_pos), old_chunk_position(c_pos), m_height(&hm) {
    this->id = m_reg->create();
    this->newid = this->id;
};

Chunk::Chunk()
{

}
Chunk& Chunk::operator=(const Chunk& other) {
    this->m_reg = other.m_reg;
    this->m_world = other.m_world;
    this->m_height = other.m_height;
    this->m_wrap = other.m_wrap;
    this->chunk_position = other.chunk_position;
    this->old_chunk_position = other.old_chunk_position;
    this->id = other.id;
    this->newid = other.newid;
    return *this;
}

void Chunk::move_to(IntTup ipos)
{
    this->chunk_position = glm::vec2(ipos.x, ipos.z);
    IntTup thetup(chunk_position.x, chunk_position.y);
    if(Chunk::donespots.find(thetup) != Chunk::donespots.end())
    {
        Chunk::donespots.erase(thetup);
    }
}
void Chunk::setx(int x)
{
    this->chunk_position.x = x;
}
void Chunk::setz(int z)
{
    this->chunk_position.y = z;
}

Chunk& Chunk::rebuild()
{
    std::vector<GLfloat> verts;
    std::vector<GLfloat> cols;
    std::vector<GLfloat> uvs;
    int half = CHUNK_WIDTH / 2;
    TextureFace face(4,3);
    TextureFace heightface(2,14);
    IntTup tup(0,0,0);
    IntTup thetup(chunk_position.x, chunk_position.y);
    if(Chunk::donespots.find(thetup) != Chunk::donespots.end()) {
        Chunk::donespots.erase(thetup);
    }
    for(int i = -half; i < half; ++i)
    {

        for(int k = -half; k < half; ++k)
        {
            tup.set(
                (int)(this->chunk_position.x * CHUNK_WIDTH) + i,
                0,
                (int)(this->chunk_position.y * CHUNK_WIDTH) + k
            );



            unsigned char fly = height_noise_func(tup);
            unsigned char fry = height_noise_func(tup + IntTup(1,0));
            unsigned char bry = height_noise_func(tup + IntTup(1,1));
            unsigned char bly = height_noise_func(tup + IntTup(0,1));
            //std::cout << std::to_string(fly) << std::endl;
            verts.insert(verts.end(), {
                static_cast<float>(tup.x), static_cast<float>(fly), static_cast<float>(tup.z),
                static_cast<float>(tup.x + 1), static_cast<float>(fry), static_cast<float>(tup.z),
                static_cast<float>(tup.x + 1),static_cast<float>(bry), static_cast<float>(tup.z + 1),

                static_cast<float>(tup.x + 1), static_cast<float>(bry), static_cast<float>(tup.z + 1),
                static_cast<float>(tup.x), static_cast<float>(bly), static_cast<float>(tup.z + 1),
                static_cast<float>(tup.x), static_cast<float>(fly), static_cast<float>(tup.z),
            });





            float flb = 1.0f;
            float frb = 1.0f;
            float brb = 1.0f;
            float blb = 1.0f;

            float dark = 0.6f;

            float slantbottom = 0.3f;

            for(int b = static_cast<int>(fly)+2; b < CHUNK_HEIGHT; ++b) {
                IntTup t(tup.x, b, tup.z);
                if(block_noise_func(t)) {
                    flb = dark;

                    break;
                }

            }
            if(fly < fry || fly < bry || fly < bly) {
                flb -= slantbottom;
            }
            for(int b = static_cast<int>(fry)+2; b < CHUNK_HEIGHT; ++b) {
                IntTup t(tup.x+1, b, tup.z);
                if(block_noise_func(t)) {
                    frb = dark;

                    break;
                }

            }
            if(fry < fly || fry < bry || fry < bly) {
                frb -= slantbottom;
            }
            for(int b = static_cast<int>(bry)+2; b < CHUNK_HEIGHT; ++b) {
                IntTup t(tup.x+1, b, tup.z+1);
                if(block_noise_func(t)) {
                    brb = dark;

                    break;
                }

            }
            if(bry < fly || bry < fry || bry < bly) {
                brb -= slantbottom;
            }
            for(int b = static_cast<int>(bly)+2; b < CHUNK_HEIGHT; ++b) {
                IntTup t(tup.x, b, tup.z+1);
                if(block_noise_func(t)) {
                    blb = dark;

                    break;
                }

            }
            if(bly < fly || bly < fry || bly < bry) {
                blb -= slantbottom;
            }

            cols.insert(cols.end(), {
                flb,flb,flb,
                frb,frb,frb,
                brb,brb,brb,

                brb,brb,brb,
                blb,blb,blb,
                flb,flb,flb,
            });
            uvs.insert(uvs.end(), {
                heightface.bl.x, heightface.bl.y,
                heightface.tl.x, heightface.tl.y,
                heightface.tr.x, heightface.tr.y,

                heightface.tr.x, heightface.tr.y,
                heightface.br.x, heightface.br.y,
                heightface.bl.x, heightface.bl.y,
            });


            //Blocks
            for(int j = 0; j < CHUNK_HEIGHT; ++j)
            {
                tup.set(
                    tup.x,
                    0+j,
                    tup.z
                );

                bool solid = block_noise_func(tup);

                if(solid)
                {
                    for(Neighbor& neighbor : get_neighbors(tup))
                    {
                        if(!block_noise_func(neighbor.tup))
                        {
                            Cube::stamp_face(neighbor.face, tup.x, tup.y, tup.z, face, verts, cols, uvs);
                        }
                    }
                }
            }
        }
    }
    //std::cout << verts.size() << " " << cols.size() << " " << uvs.size() << std::endl;

    if (!m_reg->all_of<MeshComponent>(this->newid))
    {
        //std::cout << "You dont have a mesh component" << std::endl;
        MeshComponent m;
        m.length = verts.size();
        m_wrap->bindGeometry(
            m.vbov,
            m.vboc,
            m.vbouv,
            verts.data(),
            cols.data(),
            uvs.data(),
            verts.size() * sizeof(GLfloat),
            cols.size() * sizeof(GLfloat),
            uvs.size() * sizeof(GLfloat)
        );
        m_reg->emplace<MeshComponent>(this->newid, m);
    }
    else {
        //std::cout << "You have a mesh component" << std::endl;
        MeshComponent& m = m_reg->get<MeshComponent>(this->newid);

        glDeleteBuffers(1, &m.vbov);
        glDeleteBuffers(1, &m.vbouv);
        glDeleteBuffers(1, &m.vboc);
        glGenBuffers(1, &m.vbov);
        glGenBuffers(1, &m.vbouv);
        glGenBuffers(1, &m.vboc); //delete and re-gen because the buffer might be different sized

        m.length = verts.size();
        m_wrap->bindGeometry(
            m.vbov,
            m.vboc,
            m.vbouv,
            verts.data(),
            cols.data(),
            uvs.data(),
            verts.size() * sizeof(GLfloat),
            cols.size() * sizeof(GLfloat),
            uvs.size() * sizeof(GLfloat)
        );
    }
    this->id = this->newid;
    this->old_chunk_position = this->chunk_position;

    Chunk::donespots.insert_or_assign(thetup, 0);
    return *this;
}

#endif
