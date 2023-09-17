#pragma once
#include "glwrapper.hpp"
#include <vector>
#include "inttup.hpp"
#include <unordered_map>
#include <entt/entt.hpp>
#include "cube_helper.hpp"
#include "mesh_component.hpp"
#include "textureface.hpp"
#include "perlin_stuff.hpp"
class BoundingBox
{
public:
    BoundingBox(const glm::vec3 &m_min_corner, const glm::vec3 &m_max_corner, const glm::vec3 &collision_normal);
    BoundingBox(const glm::vec3 &center, const glm::vec3 &collision_normal);
    void set_center(const glm::vec3 &center);
    void set_center(const glm::vec3 &center, float yextent, float xextent);
    bool intersects(const BoundingBox &other) const;
    double BoundingBox::get_penetration(const BoundingBox &other) const;
    glm::vec3 collision_normal;
    glm::vec3 center;

private:
    glm::vec3 m_min_corner;
    glm::vec3 m_max_corner;
};

#ifdef COLLCAGE_IMP

double BoundingBox::get_penetration(const BoundingBox &other) const
{
    if (!intersects(other))
    {
        return 0.0;
    }

    double x_penetration = std::min(m_max_corner.x - other.m_min_corner.x, other.m_max_corner.x - m_min_corner.x);
    double y_penetration = std::min(m_max_corner.y - other.m_min_corner.y, other.m_max_corner.y - m_min_corner.y);
    double z_penetration = std::min(m_max_corner.z - other.m_min_corner.z, other.m_max_corner.z - m_min_corner.z);

    return std::min({x_penetration, y_penetration, z_penetration});
}

BoundingBox::BoundingBox(const glm::vec3 &m_min_corner, const glm::vec3 &m_max_corner, const glm::vec3 &collision_normal)
    : m_min_corner(m_min_corner), m_max_corner(m_max_corner), collision_normal(collision_normal), center(glm::mix(m_max_corner, m_min_corner, 0.5f))
{
}

BoundingBox::BoundingBox(const glm::vec3 &center, const glm::vec3 &collision_normal)
    : m_min_corner(center + glm::vec3(-0.5, -0.5, -0.5)), m_max_corner(center + glm::vec3(0.5, 0.5, 0.5)), collision_normal(collision_normal), center(center)
{
}

void BoundingBox::set_center(const glm::vec3 &center)
{
    this->m_min_corner = center + glm::vec3(-0.5, -0.5, -0.5);
    this->m_max_corner = center + glm::vec3(0.5, 0.5, 0.5);
    this->center = center;
}

void BoundingBox::set_center(const glm::vec3 &center, float yextent, float xextent)
{
    this->m_min_corner = center + glm::vec3(-xextent, -yextent, -xextent);
    this->m_max_corner = center + glm::vec3(xextent, yextent, xextent);
    this->center = center;
}

bool BoundingBox::intersects(const BoundingBox &other) const
{
    return !(m_max_corner.x < other.m_min_corner.x || m_min_corner.x > other.m_max_corner.x ||
             m_max_corner.y < other.m_min_corner.y || m_min_corner.y > other.m_max_corner.y ||
             m_max_corner.z < other.m_min_corner.z || m_min_corner.z > other.m_max_corner.z);
}

#endif

enum Side
{
    ROOF = 0,
    FLOOR,

    LEFTTOP,
    LEFTBOTTOM,

    RIGHTTOP,
    RIGHTBOTTOM,

    FRONTTOP,
    FRONTBOTTOM,

    BACKTOP,
    BACKBOTTOM,

    BACKRIGHTTOP,
    BACKRIGHTBOTTOM,

    BACKLEFTTOP,
    BACKLEFTBOTTOM,

    FRONTRIGHTTOP,
    FRONTRIGHTBOTTOM,

    FRONTLEFTTOP,
    FRONTLEFTBOTTOM,

    INBOTTOM,
    INTOP,

    BOTTOMFRONTEDGE,
    BOTTOMLEFTEDGE,
    BOTTOMRIGHTEDGE,
    BOTTOMBACKEDGE

};

#define NUMBER_OF_BOXES 24

class CollisionCage
{
public:
    std::vector<Side> colliding;
    std::vector<Side> solid;
    glm::ivec3 position;
    std::array<BoundingBox, NUMBER_OF_BOXES> boxes;
    std::array<double, NUMBER_OF_BOXES> penetration;
    std::array<entt::entity, NUMBER_OF_BOXES> entities;
    static const glm::vec3 normals[NUMBER_OF_BOXES];
    static const glm::ivec3 positions[NUMBER_OF_BOXES];
    CollisionCage(GLWrapper &wr, entt::registry &reg);
    void update_position(glm::vec3 &pos);
    void update_solidity();
    void update_colliding(BoundingBox &user);
    void debug_display();
    void update_readings(glm::vec3 &pos);

private:
    GLWrapper &m_wrap;
    entt::registry &m_reg;
};

#ifdef COLLCAGE_IMP

void CollisionCage::debug_display()
{
    for (const auto &id : entities)
    {
        if (m_reg.all_of<MeshComponent>(id))
        {
            MeshComponent &m = m_reg.get<MeshComponent>(id);
            m.length = 0;
        }
    }

    for (Side side : this->solid)
    {
        // Make the model
        std::vector<GLfloat> verts;
        std::vector<GLfloat> cols;
        std::vector<GLfloat> uvs;

        glm::vec3 cent = boxes[side].center;

        for (int i = 0; i < 6; ++i)
        {
            Cube::stamp_face(static_cast<CubeFace>(i),
                             cent.x, cent.y, cent.z,
                             TextureFace(10,10),
                             verts,
                             cols,
                             uvs);
        }

        if (!m_reg.all_of<MeshComponent>(entities[side]))
        {
            MeshComponent m;
            m.length = verts.size();
            m_wrap.bindGeometry(
                m.vbov,
                m.vboc,
                m.vbouv,
                verts.data(),
                cols.data(),
                uvs.data(),
                verts.size() * sizeof(GLfloat),
                cols.size() * sizeof(GLfloat),
                uvs.size() * sizeof(GLfloat));
            m_reg.emplace<MeshComponent>(entities[side], m);
        }
        else
        {
            MeshComponent &m = m_reg.get<MeshComponent>(entities[side]);
            m.length = verts.size();
            m_wrap.bindGeometry(
                m.vbov,
                m.vboc,
                m.vbouv,
                verts.data(),
                cols.data(),
                uvs.data(),
                verts.size() * sizeof(GLfloat),
                cols.size() * sizeof(GLfloat),
                uvs.size() * sizeof(GLfloat));
        }
    }
}

void CollisionCage::update_position(glm::vec3 &pos)
{
    this->position = glm::ivec3(static_cast<int>(std::round(pos.x)), static_cast<int>(std::round(pos.y)), static_cast<int>(std::round(pos.z)));
    for (int i = 0; i < 18; ++i)
    {
        this->boxes[i].set_center(CollisionCage::positions[i] + this->position);
    }
}
void CollisionCage::update_solidity()
{
    this->solid.clear();

    for (int i = 0; i < 18; ++i)
    {
        glm::vec3 spot = this->boxes[i].center;
        IntTup tup(spot.x, spot.y, spot.z);
        Side side = static_cast<Side>(i);
        if (block_noise_func(tup))
        {
            if(std::find(this->solid.begin(), this->solid.end(), side) == solid.end())
            {

                this->solid.push_back(side);
            }

        }
        else
        {
            this->solid.erase(std::remove(solid.begin(), solid.end(), side), solid.end());
        }
    }
}
void CollisionCage::update_colliding(BoundingBox &user)
{
    //RESET
    this->colliding.clear(); //Clear colliding and penetration
    for(int i = 0; i < 18; ++i)
    {
        this->penetration[i] = 0.0;
    }

    //RE-ASSESS
    for(Side& side : this->solid) //Look through solid boxes
    {


        if(user.intersects(this->boxes[side]))
        {
            if(std::find(colliding.begin(), colliding.end(), side) == colliding.end())
            {


                this->colliding.push_back(side);//Add to colliding
            }

            //std::cout << "side id: " << static_cast<int>(side) << std::endl;
            this->penetration[side] = user.get_penetration(boxes[side]); //Set penetration amount
        }
    }
    //std::cout << "colliding size: " <<  this->colliding.size() << std::endl;
}

CollisionCage::CollisionCage(GLWrapper &wr, entt::registry &reg)
    : m_wrap(wr), m_reg(reg), boxes{
    BoundingBox(CollisionCage::positions[0], CollisionCage::normals[0]),
    BoundingBox(CollisionCage::positions[1], CollisionCage::normals[1]),
    BoundingBox(CollisionCage::positions[2], CollisionCage::normals[2]),
    BoundingBox(CollisionCage::positions[3], CollisionCage::normals[3]),
    BoundingBox(CollisionCage::positions[4], CollisionCage::normals[4]),
    BoundingBox(CollisionCage::positions[5], CollisionCage::normals[5]),
    BoundingBox(CollisionCage::positions[6], CollisionCage::normals[6]),
    BoundingBox(CollisionCage::positions[7], CollisionCage::normals[7]),
    BoundingBox(CollisionCage::positions[8], CollisionCage::normals[8]),
    BoundingBox(CollisionCage::positions[9], CollisionCage::normals[9]),

    BoundingBox(CollisionCage::positions[10], CollisionCage::normals[10]),
    BoundingBox(CollisionCage::positions[11], CollisionCage::normals[11]),
    BoundingBox(CollisionCage::positions[12], CollisionCage::normals[12]),
    BoundingBox(CollisionCage::positions[13], CollisionCage::normals[13]),
    BoundingBox(CollisionCage::positions[14], CollisionCage::normals[14]),
    BoundingBox(CollisionCage::positions[15], CollisionCage::normals[15]),
    BoundingBox(CollisionCage::positions[16], CollisionCage::normals[16]),
    BoundingBox(CollisionCage::positions[17], CollisionCage::normals[17]),
    BoundingBox(CollisionCage::positions[18], CollisionCage::normals[18]),
    BoundingBox(CollisionCage::positions[19], CollisionCage::normals[19]),

    BoundingBox(CollisionCage::positions[20], CollisionCage::normals[20]),
    BoundingBox(CollisionCage::positions[21], CollisionCage::normals[21]),
    BoundingBox(CollisionCage::positions[22], CollisionCage::normals[22]),
    BoundingBox(CollisionCage::positions[23], CollisionCage::normals[23]),
}
{

    penetration = {
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, //Corners
        0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
    };
    entities = {
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),

        //Corners
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),

        m_reg.create(),
        m_reg.create(),

        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
        m_reg.create(),
    };
}

const glm::vec3 CollisionCage::normals[NUMBER_OF_BOXES] = {
    glm::vec3(0, -1, 0), // TOp/roof
    glm::vec3(0, 1, 0),  // BOTTOM/floor

    glm::vec3(1, 0, 0), // LEFTTOP
    glm::vec3(1, 0, 0), // LEFTBOTTOM

    glm::vec3(-1, 0, 0), // RIGHTTOP
    glm::vec3(-1, 0, 0), // RIGHTBOTTOM

    glm::vec3(0, 0, -1), // FRONTTOP
    glm::vec3(0, 0, -1), // FRONTBOTTOM

    glm::vec3(0, 0, 1), // BACKTOP
    glm::vec3(0, 0, 1),  // BACKBOTTOM

    glm::vec3(0, 0, 1),    //BACKRIGHTTOP,
    glm::vec3(0, 0, 1),  //BACKRIGHTBOTTOM,

    glm::vec3(0, 0, 1),//BACKLEFTTOP,
    glm::vec3(0, 0, 1),//BACKLEFTBOTTOM,

    glm::vec3(0, 0, -1),//FRONTRIGHTTOP,
    glm::vec3(0, 0, -1),//FRONTRIGHTBOTTOM,

    glm::vec3(0, 0, -1),//FRONTRIGHTTOP,
    glm::vec3(0, 0, -1),//FRONTRIGHTBOTTOM,

    glm::vec3(0, 1, 0), //INBOTTOM
    glm::vec3(0, 1, 0), //INTOP

    glm::vec3(0, 0, -1),
    glm::vec3(1, 0, 0),
    glm::vec3(-1, 0, 0),
    glm::vec3(0, 0, 1),

};

const glm::ivec3 CollisionCage::positions[NUMBER_OF_BOXES] = {
    glm::ivec3(0, 2, 0),  // TOp/roof
    glm::ivec3(0, -1, 0), // BOTTOM/floor

    glm::ivec3(-1, 1, 0), // LEFTTOP
    glm::ivec3(-1, 0, 0), // LEFTBOTTOM

    glm::ivec3(1, 1, 0), // RIGHTTOP
    glm::ivec3(1, 0, 0), // RIGHTBOTTOM

    glm::ivec3(0, 1, 1), // FRONTTOP
    glm::ivec3(0, 0, 1), // FRONTBOTTOM

    glm::ivec3(0, 1, -1), // BACKTOP
    glm::ivec3(0, 0, -1),  // BACKBOTTOM

    glm::ivec3(1, 1, -1),    //BACKRIGHTTOP,
    glm::ivec3(1, 0, -1),//BACKRIGHTBOTTOM,

    glm::ivec3(-1, 1, -1),//BACKLEFTTOP,
    glm::ivec3(-1, 0, -1),//BACKLEFTBOTTOM,

    glm::ivec3(1, 1, 1),//FRONTRIGHTTOP,
    glm::ivec3(1, 0, 1),//FRONTRIGHTBOTTOM,

    glm::ivec3(-1, 1, 1),//FRONTLEFTTOP,
    glm::ivec3(-1, 0, 1),//FRONTLEFTBOTTOM


    glm::ivec3(0, 0, 0), //inbottom
    glm::ivec3(0, 1, 0), //intop

    glm::ivec3(0, -1, 1),
    glm::ivec3(-1, -1, 0),
    glm::ivec3(1, -1, 0),
    glm::ivec3(0, -1, -1),
};

void CollisionCage::update_readings(glm::vec3 &pos)
{
    this->update_position(pos);
    this->update_solidity();
    //this->debug_display();
}

#endif
