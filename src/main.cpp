#pragma once
#define JACKS_GL_WRAPPER_IMPLEMENTATION
#include "glwrapper.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TEXTURE_FACE_IMPLEMENTATION
#include "textureface.hpp"

#define INTTUP_IMPLEMENTATION
#include "inttup.hpp"

#define CUBE_HELPER_IMPLEMENTATION
#include "cube_helper.hpp"


#define NEIGHBORS_IMPLEMENTATION
#include "inttup_neighbor.hpp"

#define GRAV 9.81f


#define COMPONENT_IMPLEMENTATIONS
#include "mesh_component.hpp"



#define CHUNK_IMPLEMENTATION
#include "chunk_handler.hpp"

#define PERLIN_IMP
#include "perlin.h"

#define WORLDGEN_IMP
#include "worldgen.hpp"

#define COLLCAGE_IMP
#include "collision_cage.hpp"

#define GUITEXT_IMP
#include "gui_text.hpp"

#define CHUNKFORMATION_IMP
#include "chunk_formation.hpp"

#include <entt/entt.hpp>
#include <cstdlib>

unsigned int texture;
GLWrapper wrap;
int waterHeight = -200;

std::unordered_map<IntTup, int, IntTupHash> worldmap;

std::unordered_map<IntTup, float, IntTupHash> heightmap;


entt::registry registry;

CollisionCage cage(wrap, worldmap, registry);

ChunkFormation cformation(wrap, worldmap, registry, heightmap);



void prepare_texture();

std::string show_vars;

bool grounded = false;



int main() {


    wrap.initializeGL();
    wrap.setupVAO();
    wrap.cameraPos = glm::vec3(0,90,0);
    prepare_texture();

    glClearColor(0.5f, 0.5f, 0.8f, 1.0f);
    float deltaTime = 0;
    float lastFrame = 0;

    int prevUwv = 0;
    generate_world(worldmap, heightmap);

    float user_width_half = 0.2f;


    /*for(int i = -world_width/2; i < world_width/2; ++i)
    {
        for(int k = -world_width/2; k < world_width/2; ++k)
        {
            world[i + world_width/2][k+world_width/2] = Chunk(glm::vec2(i,k), registry, wrap, worldmap).rebuild();
        }
    }*/



    BoundingBox user(wrap.cameraPos, glm::vec3(0,0,0));

    auto meshes_view = registry.view<MeshComponent>();


    while (!glfwWindowShouldClose(wrap.window))
    {
        cformation.set_position(wrap.cameraPos);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (wrap.activeState.forward)
        {
            wrap.activeState.forwardVelocity = std::min(wrap.activeState.forwardVelocity + (25.0f*deltaTime), 5.0f);
        } else {
            wrap.activeState.forwardVelocity *= .400f;
        }
        //update_show_vars();
        glm::vec3 collision_cage_center = wrap.cameraPos + glm::vec3(0, -1.0, 0);
        cage.update_readings(collision_cage_center);
        if(wrap.activeState.jump && grounded)
        {
            wrap.activeState.upVelocity += 5.0f;
            grounded = false;
            wrap.activeState.jump = false;
        }

















        if(!grounded)
        {
            wrap.activeState.upVelocity = std::max(wrap.activeState.upVelocity + ((GRAV * -deltaTime) * (deltaTime*10)), -(GRAV * deltaTime));
            wrap.activeState.upVelocity = std::min(wrap.activeState.upVelocity, (GRAV * deltaTime)*2.0f);
        }

        if (wrap.activeState.forwardVelocity != 0 || wrap.activeState.upVelocity != 0)
        {

            glm::vec3 up_dir(0,1,0);
            glm::vec3 forward_dir = wrap.cameraDirection;

            forward_dir.y = 0;
            //wrap.cameraPos += (dir * wrap.activeState.forwardVelocity) * 0.65f;
            glm::vec3 upward_change = (up_dir * wrap.activeState.upVelocity);
            glm::vec3 forward_change = ((forward_dir * wrap.activeState.forwardVelocity) * deltaTime);
            glm::vec3 desired_movement = upward_change + forward_change;
            //std::cout << "DESIRED MOVEMENT: " << desired_movement.x << " " << desired_movement.y << " " << desired_movement.z << std::endl;
            glm::vec3 user_center = wrap.cameraPos + glm::vec3(0, -0.5, 0);
            //std::cout << "USER CENTER " << user_center.x << " " << user_center.y << " " << user_center.z << std::endl;
            //std::cout << "USER CENTER WITH RAW MOVE APPLIED: " << (user_center + desired_movement).x << " " << (user_center + desired_movement).y << " " << (user_center + desired_movement).z << std::endl;
            std::vector<glm::vec3> corrections_made;

            for(float k = 0; k < 1.1f; k += .1f)
            {
                user.set_center(glm::mix(user_center, user_center + desired_movement, k),  0.85f, user_width_half);

                cage.update_colliding(user);

                if(cage.colliding.size() > 0)
                {
                    corrections_made.clear();
                    for(Side& side : cage.colliding)
                    {

                        if(std::find(corrections_made.begin(), corrections_made.end(), CollisionCage::normals[side]) == corrections_made.end())
                        {
                            desired_movement += CollisionCage::normals[side] * (float)cage.penetration[side];
                            corrections_made.push_back(CollisionCage::normals[side]);
                        }
                        //std::cout << "penet: " << cage.penetration[side] << std::endl;

                        if(side == FLOOR)
                        {
                            //std::cout << "FLOOR";
                            wrap.activeState.upVelocity = 0;
                            desired_movement -= upward_change;
                            grounded = true;
                        }
                        //std::cout << "change being made: " << (CollisionCage::normals[side] * (float)cage.penetration[side]).x << " " << (CollisionCage::normals[side] * (float)cage.penetration[side]).y << " " << (CollisionCage::normals[side] * (float)cage.penetration[side]).z << std::endl;
                    }

                }
            }
            if(glm::distance(user_center, user_center + desired_movement) < 4.0f) {
                user.set_center(user_center + desired_movement, 0.85f, user_width_half);
            }

            IntTup camTup((int)std::floor(user.center.x), (int)std::floor(user.center.z));
            float xi = std::floor(user_center.x);
            float zi = std::floor(user_center.z);


            float tx = user_center.x - xi;
            float tz = user_center.z - zi;

            float q11 = heightmap.at(camTup);
            float q12 = heightmap.at(camTup + IntTup(1, 0));
            float q21 = heightmap.at(camTup + IntTup(0, 1));

            float q22 = heightmap.at(camTup + IntTup(1, 1));
            float height = glm::mix(glm::mix(q11, q12, tx), glm::mix(q21, q22, tx), tz);

            //if(glm::distance(user.center + glm::vec3(0, 0.5, 0),wrap.cameraPos) < 0.5f )
            if((user.center + glm::vec3(0, -1, 0)).y > height) {
                wrap.cameraPos = user.center + glm::vec3(0, 0.5, 0);
                if(std::find(cage.solid.begin(), cage.solid.end(), FLOOR) == cage.solid.end())
                {
                    grounded = false;
                }

            } else {

                glm::vec3 spot = user.center + glm::vec3(0, 0.5, 0);
                spot.y = height + 1.5f;

                wrap.cameraPos = spot;
                grounded = true;
            }


        }



        int uwV = (wrap.cameraPos.y - 0.15f < waterHeight) ? 1 : 0;


        wrap.updateOrientation();
        glBindVertexArray(wrap.vao);
        glUseProgram(wrap.shaderProgram);

        GLuint mvpLoc = glGetUniformLocation(wrap.shaderProgram, "mvp");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(wrap.mvp));

        GLuint camPosLoc = glGetUniformLocation(wrap.shaderProgram, "camPos");
        glUniform3f(camPosLoc, wrap.cameraPos.x, wrap.cameraPos.y, wrap.cameraPos.z);

        GLuint uwLoc = glGetUniformLocation(wrap.shaderProgram, "underWater");
        // int uwFeet = (wrap.cameraPos.y - 2 < waterHeight) ? 1 : 0;
        glUniform1i(uwLoc, uwV);
//draw here


        for (const entt::entity entity : meshes_view)
        {
            MeshComponent& m = registry.get<MeshComponent>(entity);
            wrap.bindGeometryNoUpload(
                m.vbov,
                m.vboc,
                m.vbouv);

            glDrawArrays(GL_TRIANGLES, 0, m.length);

        }
        draw_text((std::string("Wellrc v0.1\n")).c_str(), -0.95f, 0.95f);


        //std::cout << wrap.cameraPos.x << " " << wrap.cameraPos.y << " " << std::endl;
        glBindVertexArray(0);
        glfwSwapBuffers(wrap.window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        wrap.deltaTime = deltaTime;
        lastFrame = currentFrame;

        /*if (wrap.activeState.forwardVelocity > 0)
        {
            auto dir = wrap.cameraDirection;
            dir.y = 0;
            wrap.cameraPos += (dir * wrap.activeState.forwardVelocity) * 3.0f;
            wrap.activeState.forwardVelocity *= friction;
        }*/
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void prepare_texture()
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("src/assets/texture.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}
