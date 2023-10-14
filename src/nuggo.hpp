#pragma once

#include <vector>
#include <gl/GL.h>
#include <entt/entt.hpp>


class Nuggo {
public:
    std::vector<GLfloat> verts;
    std::vector<GLfloat> cols;
    std::vector<GLfloat> uvs;
    entt::entity me;
};

