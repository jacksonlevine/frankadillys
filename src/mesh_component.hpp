#pragma once
#include "GL/glew.h"

struct MeshComponent {
public:
    GLuint vbov;
    GLuint vboc;
    GLuint vbouv;
    int length;
    MeshComponent();
};

#ifdef COMPONENT_IMPLEMENTATIONS

MeshComponent::MeshComponent() : length(0) {
    glGenBuffers(1, &this->vbov);
    GLenum error1 = glGetError();
    if (error1 != GL_NO_ERROR) {
        std::cerr << "OpenGL error after glGenBuffers vbov: " << error1 << std::endl;
    }

    glGenBuffers(1, &this->vboc);
    GLenum error2 = glGetError();
    if (error2 != GL_NO_ERROR) {
        std::cerr << "OpenGL error after glGenBuffers vboc: " << error2 << std::endl;
    }

    glGenBuffers(1, &this->vbouv);
    GLenum error3 = glGetError();
    if (error3 != GL_NO_ERROR) {
        std::cerr << "OpenGL error after glGenBuffers vbouv: " << error3 << std::endl;
    }
}

#endif