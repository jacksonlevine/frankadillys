#pragma once
#include <iostream>
#include <format>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct InputState
{
    bool forward;
    bool left;
    bool right;
    bool back;
    bool jump;
    float forwardVelocity;
    float upVelocity;
    float rightVelocity;
};

class GLWrapper
{
public:
    glm::vec3 cameraPos;
    glm::vec3 cameraTarget;
    glm::vec3 cameraDirection;
    glm::vec3 up;
    glm::vec3 cameraRight;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float cameraSpeed;
    float cameraYaw;
    float cameraPitch;
    float lastX, lastY;
    bool firstMouse;
    glm::mat4 view;
    glm::mat4 model;

    int wi;
    int he;

    glm::mat4 projection;
    glm::mat4 mvp;
    glm::vec3 direction;
    GLuint shaderProgram;
    GLuint vao;
    GLFWwindow *window;
    float deltaTime;
    InputState activeState;
    GLWrapper();
    int initializeGL();
    void setFOV(int newF);
    void bindGeometry(GLuint vbov, GLuint vboc, GLuint vbouv, const GLfloat *vertices, const GLfloat *colors, const GLfloat *uv, int vsize, int csize, int usize);
    void bindGeometryNoUpload(GLuint vbov, GLuint vboc, GLuint vbouv);
    void setupVAO();
    void bindVAO();
    void updateOrientation();
    static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static GLWrapper *instance;
};

#ifdef JACKS_GL_WRAPPER_IMPLEMENTATION

GLWrapper *GLWrapper::instance = nullptr;

GLWrapper::GLWrapper()
{
    // Camera position and rotation
    this->cameraPos = glm::vec3(0.0f, 7.0f, 0.0f);
    this->cameraTarget = glm::vec3(0.0f, 7.0f, -3.0f);
    this->cameraDirection = glm::normalize(cameraPos - cameraTarget);
    this->up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    this->cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    this->cameraUp = glm::cross(cameraDirection, cameraRight);
    this->cameraSpeed = 0.1f;
    this->cameraYaw = -90.0f;
    this->cameraPitch = 0.0f;
    this->lastX = 400;
    this->lastY = 300;
    this->firstMouse = true;
    this->deltaTime = 0;
    this->view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    this->vao = 0;
    instance = this;
}
bool captured = false;
void GLWrapper::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (instance && captured)
    {
        if (instance->firstMouse)
        {
            instance->lastX = xpos;
            instance->lastY = ypos;
            instance->firstMouse = false;
        }

        float xoffset = xpos - instance->lastX;
        float yoffset = instance->lastY - ypos;
        instance->lastX = xpos;
        instance->lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        instance->cameraYaw += xoffset;
        instance->cameraPitch += yoffset;

        if (instance->cameraPitch > 89.0f)
            instance->cameraPitch = 89.0f;
        if (instance->cameraPitch < -89.0f)
            instance->cameraPitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(instance->cameraYaw)) * cos(glm::radians(instance->cameraPitch));
        front.y = sin(glm::radians(instance->cameraPitch));
        front.z = sin(glm::radians(instance->cameraYaw)) * cos(glm::radians(instance->cameraPitch));
        instance->cameraFront = glm::normalize(front);
    }
}

void GLWrapper::mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (instance)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            if (!captured)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                instance->firstMouse = true;
                captured = true;
            }
            else
            {
                // Game::instance->onLeftClick();
            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            // Game::instance->onRightClick();
        }
    }
}

void GLWrapper::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (instance)
    {
        if (key == GLFW_KEY_W)
        {
            if (action == GLFW_PRESS)
                instance->activeState.forward = true;
            if (action == GLFW_RELEASE)
                instance->activeState.forward = false;
        }
        if (key == GLFW_KEY_SPACE)
        {
            if (action == GLFW_PRESS)
                instance->activeState.jump = true;
            if (action == GLFW_RELEASE)
                instance->activeState.jump = false;
        }
        if (key == GLFW_KEY_ESCAPE)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            instance->firstMouse = true;
            captured = false;
        }
    }
}

void GLWrapper::setFOV(int newFOV)
{

    projection = glm::perspective(glm::radians((float)newFOV), (float)this->wi / (float)this->he, 0.1f, 5000.0f);
}

int GLWrapper::initializeGL()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // this->wi = 1900;
    // this->he = 1000;

    this->wi = 1900;
    this->he = 1060;

    // Set up GLFW window hints
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    window = glfwCreateWindow(wi, he, "Wellrc v0.1", NULL, NULL);
    if (!this->window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(this->window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set up the viewport
    glViewport(0, 0, wi, he);

    // Enable depth testing
    // glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glDepthFunc(GL_LESS);
    // Set up GLM for 3D math
    model = glm::mat4(1.0f);

    projection = glm::perspective(glm::radians(75.0f), (float)wi / (float)he, 0.1f, 5000.0f);

    // Enable pointer-locking first-person controls
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Set up event callbacks
    glfwSetCursorPosCallback(this->window, GLWrapper::mouse_callback);
    glfwSetMouseButtonCallback(this->window, GLWrapper::mouse_button_callback);

    glfwSetKeyCallback(this->window, GLWrapper::keyCallback);
    // Create vertex shader object
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Set vertex shader source code
    const GLchar *vertexShaderSource =
        "#version 450 core\n"
        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 color;\n"
        "layout (location = 2) in vec2 uv;\n"
        "out vec3 vertexColor;\n"
        "out vec2 TexCoord;\n"
        "uniform mat4 mvp;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = mvp * vec4(position, 1.0);\n"
        "    vertexColor = color;\n"
        "    TexCoord = uv;\n"
        "}\n";
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // Compile vertex shader
    glCompileShader(vertexShader);

    // Check vertex shader compilation errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation error: " << infoLog << std::endl;
    }

    // Create fragment shader object
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Set fragment shader source code
    const GLchar *fragmentShaderSource =
        "#version 450 core\n"
        "in vec3 vertexColor;\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D ourTexture;\n"
        "uniform vec3 camPos;\n"
        "uniform int underWater;"
        "void main()\n"
        "{\n"
        "vec4 texColor = texture(ourTexture, TexCoord);\n"
        "if(texColor.a < 0.1){\n"
        "discard;}\n"

        // Calculate the distance between fragment and camera

        "       vec3 fogColor = vec3(0.2, 0.2, 0.7);\n" // Adjust the fog color as desired

        "float diss = pow(     gl_FragCoord.z , 2);\n"
        "if(underWater == 0) {\n"
        "if(gl_FragCoord.z < 0.9978f) { diss = 0; } else { diss = (diss-0.9978f)*1000; }  \n"
        "} else { diss = (diss-0.6)*4; } \n"
        "    vec3 finalColor = mix(vertexColor, fogColor, max(diss/4.0, 0));\n"
        "    FragColor = mix(vec4(finalColor, 1.0) * texColor, vec4(fogColor, 1.0), max(diss/4.0, 0));\n"
        "}\n";
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    // Compile fragment shader
    glCompileShader(fragmentShader);

    // Check fragment shader compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation error: " << infoLog << std::endl;
    }

    // Create shader program object store it on this.shaderProgram
    this->shaderProgram = glCreateProgram();

    // Attach vertex and fragment shaders to shader program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link shader program
    glLinkProgram(shaderProgram);

    // Check shader program linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
    }

    // Delete shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void GLWrapper::setupVAO()
{
    // Generate a vertex array object (VAO)

    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    // Use our shader program
    glUseProgram(this->shaderProgram);
}

void GLWrapper::bindVAO()
{
    glBindVertexArray(this->vao);

    // Use our shader program
    glUseProgram(this->shaderProgram);
}
void GLWrapper::bindGeometry(GLuint vbov, GLuint vboc, GLuint vbouv, const GLfloat *vertices, const GLfloat *colors, const GLfloat *uv, int vsize, int csize, int usize)
{

    // Generate a vertex buffer object (VBO) for the position data
    GLenum error;

    glBindBuffer(GL_ARRAY_BUFFER, vbov);
    glBufferData(GL_ARRAY_BUFFER, vsize, vertices, GL_STATIC_DRAW);

    error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error after glBindBuffer and glBufferData (vbov): " << error << std::endl;
    }
    // Set up the vertex attribute pointers for the position buffer object
    GLint pos_attrib = glGetAttribLocation(this->shaderProgram, "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    // Generate a vertex buffer object (VBO) for the color data
    glBindBuffer(GL_ARRAY_BUFFER, vboc);
    glBufferData(GL_ARRAY_BUFFER, csize, colors, GL_STATIC_DRAW);
    error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error after glBindBuffer and glBufferData (vboc): " << error << std::endl;
    }
    // Set up the vertex attribute pointers for the color buffer object
    GLint col_attrib = glGetAttribLocation(this->shaderProgram, "color");
    glEnableVertexAttribArray(col_attrib);
    glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Generate a vertex buffer object (VBO) for the uv data
    glBindBuffer(GL_ARRAY_BUFFER, vbouv);
    glBufferData(GL_ARRAY_BUFFER, usize, uv, GL_STATIC_DRAW);
    error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error after glBindBuffer and glBufferData (vbouv): " << error << std::endl;
    }

    // Set up the vertex attribute pointers for the uv buffer object
    GLint uv_attrib = glGetAttribLocation(this->shaderProgram, "uv");
    glEnableVertexAttribArray(uv_attrib);
    glVertexAttribPointer(uv_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void GLWrapper::bindGeometryNoUpload(GLuint vbov, GLuint vboc, GLuint vbouv)
{

    glBindBuffer(GL_ARRAY_BUFFER, vbov);
    // Set up the vertex attribute pointers for the position buffer object
    GLint pos_attrib = glGetAttribLocation(this->shaderProgram, "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    // Generate a vertex buffer object (VBO) for the color data
    glBindBuffer(GL_ARRAY_BUFFER, vboc);

    // Set up the vertex attribute pointers for the color buffer object
    GLint col_attrib = glGetAttribLocation(this->shaderProgram, "color");
    glEnableVertexAttribArray(col_attrib);
    glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Generate a vertex buffer object (VBO) for the uv data
    glBindBuffer(GL_ARRAY_BUFFER, vbouv);

    // Set up the vertex attribute pointers for the uv buffer object
    GLint uv_attrib = glGetAttribLocation(this->shaderProgram, "uv");
    glEnableVertexAttribArray(uv_attrib);
    glVertexAttribPointer(uv_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void GLWrapper::updateOrientation()
{

    // Calculate the new direction vector based on the yaw and pitch angles

    direction.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    direction.y = sin(glm::radians(cameraPitch));
    direction.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));

    // Normalize the direction vector
    direction = glm::normalize(direction);
    this->cameraDirection = direction;
    // Set up the view matrix
    view = glm::lookAt(cameraPos, cameraPos + direction, cameraUp);

    mvp = projection * view * model;
}

#endif
