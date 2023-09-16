#pragma once
#include "glwrapper.hpp"
#include "textureface.hpp"
#include <vector>
#include <unordered_map>
#include "inttup.hpp"
#include <cmath>
#include <cfenv>
#include <climits>
#include <iostream>

enum CubeFace {
    TOP = 0, BOTTOM, LEFT, RIGHT, FRONT, BACK
};

class Cube {
public:

    static const GLfloat faces[6][18];

    static void stamp_face(CubeFace face,
                           int x, int y, int z,
                           TextureFace& tex,
                           std::vector<GLfloat>& verts,
                           std::vector<GLfloat>& cols,
                           std::vector<GLfloat>& uvs,
                           std::unordered_map<IntTup, int, IntTupHash>& worldref);
};

#ifdef CUBE_HELPER_IMPLEMENTATION

const GLfloat Cube::faces[6][18] = {
    {
        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f
    },
    {
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f
    },
    {
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,

        -0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f
    },
    {
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f
    },
    {
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f
    },
    {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,

        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f
    }
};

void Cube::stamp_face(CubeFace face,
                      int x, int y, int z,
                      TextureFace& tex,
                      std::vector<GLfloat>& verts,
                      std::vector<GLfloat>& cols,
                      std::vector<GLfloat>& uvs,
                      std::unordered_map<IntTup, int, IntTupHash>& worldref)
{

    glm::vec3 verties[6] = { glm::vec3(Cube::faces[face][0], Cube::faces[face][1], Cube::faces[face][2]),
                             glm::vec3(  Cube::faces[face][3], Cube::faces[face][4], Cube::faces[face][5]),
                             glm::vec3(  Cube::faces[face][6], Cube::faces[face][7], Cube::faces[face][8]),

                             glm::vec3( Cube::faces[face][9], Cube::faces[face][10], Cube::faces[face][11]),
                             glm::vec3( Cube::faces[face][12], Cube::faces[face][13], Cube::faces[face][14]),
                             glm::vec3(Cube::faces[face][15], Cube::faces[face][16], Cube::faces[face][17])
                           };

    glm::vec3 pos(x,y,z);


    auto vert1 =     verties[0]+ pos;
    auto vert2 =       verties[1]+ pos;
    auto vert3 =       verties[2]+ pos;
    auto vert4 =        verties[3]+ pos;
    auto vert5 =        verties[4]+ pos;
    auto vert6 =        verties[5]+ pos;

    verts.insert(verts.end(), {
        vert1.x, vert1.y, vert1.z,
        vert2.x, vert2.y, vert2.z,
        vert3.x, vert3.y, vert3.z,

        vert4.x, vert4.y, vert4.z,
        vert5.x, vert5.y, vert5.z,
        vert6.x, vert6.y, vert6.z
    });

    float bri = face == TOP ? 1.0f : face == BOTTOM ? 0.5f : (face == LEFT || face == RIGHT) ? 0.6f : 0.8f;

    std::vector<GLfloat> brichange;

    GLfloat changes[4] = {
        0.0f,
        -0.02f,
        -0.07f,
        -0.07f
    };

    for(int i = 0; i < 6; ++i) {
        glm::vec3 the_point = pos + (verties[i] * 0.5f);
        glm::vec3 outward = pos + verties[i] * 1.6f;

        int xx = static_cast<int>(std::round(outward.x));
        int zz = static_cast<int>(std::round(outward.z));
        int yy = static_cast<int>(std::ceil(outward.y));


        IntTup spots[3] = {
            IntTup(xx,yy,zz),
            IntTup(xx,yy,static_cast<int>(std::round(the_point.z))),
            IntTup(static_cast<int>(std::round(the_point.x)),yy,zz)
        };
        /*
            std::cout << "Our spot: " << std::endl;
            std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;

            for(int ii = 0; ii < 3; ii++)
          {
              std::cout << "Neighbor " << ii << ": " << std::endl;
             std::cout << spots[ii].x << " " << spots[ii].y << " " << spots[ii].z << std::endl;

            }
        */
        int count = 0;

        bool solids[3] = {
            false,
            false,
            false
        };

        for(int ii = 0; ii < 3; ++ii)
        {
            if(worldref.find(spots[ii]) != worldref.end())
            {
                count += 1;
                solids[ii] = true;
            }
        }
        //std::cout << "Count: " << count << std::endl;
        brichange.push_back(changes[count]);

    }
    /*std::cout << "brichange: " << std::endl;
    for(float s : brichange)
    {
        std::cout << s << std::endl;
    }*/

    cols.insert(cols.end(), {
        bri + brichange[0] * 4.0f,  bri + brichange[0] * 4.0f, bri + brichange[0] * 4.0f,
        bri + brichange[1] * 4.0f,  bri + brichange[1] * 4.0f, bri + brichange[1] * 4.0f,
        bri + brichange[2] * 4.0f,  bri + brichange[2] * 4.0f, bri + brichange[2] * 4.0f,

        bri + brichange[3] * 4.0f,  bri + brichange[3] * 4.0f, bri + brichange[3] * 4.0f,
        bri + brichange[4] * 4.0f,  bri + brichange[4] * 4.0f, bri + brichange[4] * 4.0f,
        bri + brichange[5] * 4.0f,  bri + brichange[5] * 4.0f, bri + brichange[5] * 4.0f



    });
    uvs.insert(uvs.end(), {
        tex.bl.x, tex.bl.y,
        tex.br.x, tex.br.y,
        tex.tr.x, tex.tr.y,

        tex.tr.x, tex.tr.y,
        tex.tl.x, tex.tl.y,
        tex.bl.x, tex.bl.y
    });
}

#endif
