#pragma once
#include "glwrapper.hpp"
#include <vector>
class GlyphFace
{
public:
    glm::vec2 tl;
    glm::vec2 tr;
    glm::vec2 bl;
    glm::vec2 br;
    void set_char(int charCode);
};


void draw_text(const char *text, float x, float y);


#ifdef GUITEXT_IMP

void GlyphFace::set_char(int charCode)
{
    static glm::vec2 font_atlas_topleft(288.0f / 544.0f, 0.0f);
    static float glyphWidth = 16.0 / 544.0;

    const int offset = charCode - 32;
    const float xOffset = (offset % 16) * glyphWidth;
    const float yOffset = (float)((int)(offset / 16)) * glyphWidth;

    this->tl = font_atlas_topleft + glm::vec2(xOffset, yOffset);
    this->tr = this->tl + glm::vec2(glyphWidth, 0.0f);
    this->br = this->tr + glm::vec2(0.0f, glyphWidth);
    this->bl = this->tl + glm::vec2(0.0f, glyphWidth);
}

void draw_text(const char *text, float x, float y)
{
    static std::string lastText;
    static std::vector<GLfloat> tverts;
    static std::vector<GLfloat> tuvs;

    bool newText = false;
    if (lastText != text)
    {
        lastText = std::string(text);
        newText = true;
    }

    glDisable(GL_DEPTH_TEST);
    static GLuint text_vao = 0;
    static GLuint text_shader = 0;
    static GLuint text_vbov = 0;
    static GLuint text_vbouv = 0;
    if (text_vao == 0)
    {
        glGenVertexArrays(1, &text_vao);
        glBindVertexArray(text_vao);
        glGenBuffers(1, &text_vbov);
        glGenBuffers(1, &text_vbouv);
        const GLchar *vs_src =
            "#version 450 core\n"
            "layout (location = 0) in vec3 position;\n"
            "layout (location = 1) in vec2 uv;\n"
            "out vec2 TexCoord;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = vec4(position, 1.0);\n"
            "    TexCoord = uv;\n"
            "}\n";
        const GLchar *fs_src =
            "#version 450 core\n"
            "in vec2 TexCoord;\n"
            "out vec4 FragColor;\n"
            "uniform sampler2D ourTexture;\n"
            "void main()\n"
            "{\n"
            "vec4 texColor = texture(ourTexture, TexCoord);\n"
            //"if(texColor.a < 0.1){\n"
            //"discard;}\n"
            "    FragColor = texColor;\n"
            "}\n";

        GLuint vs_id, fs_id;
        vs_id = glCreateShader(GL_VERTEX_SHADER);
        fs_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vs_id, 1, &vs_src, NULL);
        glShaderSource(fs_id, 1, &fs_src, NULL);
        glCompileShader(vs_id);

        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(vs_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vs_id, 512, NULL, infoLog);
            std::cerr << "Vertex shader compilation error: " << infoLog << std::endl;
        }

        glCompileShader(fs_id);

        glGetShaderiv(fs_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fs_id, 512, NULL, infoLog);
            std::cerr << "Fragment shader compilation error: " << infoLog << std::endl;
        }

        text_shader = glCreateProgram();
        glAttachShader(text_shader, vs_id);
        glAttachShader(text_shader, fs_id);
        glLinkProgram(text_shader);
        glDetachShader(text_shader, fs_id);
        glDetachShader(text_shader, vs_id);
        glDeleteShader(fs_id);
        glDeleteShader(vs_id);
    }
    glBindVertexArray(text_vao);
    glUseProgram(text_shader);

    // Set the text position
    float pen_x = x;
    float pen_y = y;

    GlyphFace glyph;

    float gWidth = 0.05f;
    if (newText == true)
    {
        glDeleteBuffers(1, &text_vbov);
        glDeleteBuffers(1, &text_vbouv);
        glGenBuffers(1, &text_vbov);
        glGenBuffers(1, &text_vbouv);
        tverts.clear();
        tuvs.clear();
        // Iterate over each character in the text
        const char *c = text;
        while (*c != '\0')
        {
            if (*c == '\n')
            {
                pen_x = x;
                pen_y -= gWidth;
            }
            else
            {
                glyph.set_char((int)*c);

                float x0 = pen_x;
                float y0 = pen_y;
                float x1 = x0 + gWidth / 2;
                float y1 = y0 - gWidth;

                tverts.insert(tverts.end(), {
                    x0,
                    y1,
                    0.3f,
                    x1,
                    y1,
                    0.3f,
                    x1,
                    y0,
                    0.3f,
                    x1,
                    y0,
                    0.3f,
                    x0,
                    y0,
                    0.3f,
                    x0,
                    y1,
                    0.3f,
                });
                tuvs.insert(tuvs.end(), {
                    glyph.bl.x,
                    glyph.bl.y,
                    glyph.br.x,
                    glyph.br.y,
                    glyph.tr.x,
                    glyph.tr.y,

                    glyph.tr.x,
                    glyph.tr.y,
                    glyph.tl.x,
                    glyph.tl.y,
                    glyph.bl.x,
                    glyph.bl.y,
                });

                // Move the pen position to the right for the next character
                pen_x += gWidth / 2.5;
            }

            // Move to the next character
            ++c;
        }
        glBindBuffer(GL_ARRAY_BUFFER, text_vbov);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * tverts.size(), &(tverts[0]), GL_STATIC_DRAW);
        // Set up the vertex attribute pointers for the position buffer object
        GLint pos_attrib = glGetAttribLocation(text_shader, "position");
        glEnableVertexAttribArray(pos_attrib);
        glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
        // Generate a vertex buffer object (VBO) for the uv data
        glBindBuffer(GL_ARRAY_BUFFER, text_vbouv);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * tuvs.size(), &(tuvs[0]), GL_STATIC_DRAW);
        // Set up the vertex attribute pointers for the uv buffer object
        GLint uv_attrib = glGetAttribLocation(text_shader, "uv");
        glEnableVertexAttribArray(uv_attrib);
        glVertexAttribPointer(uv_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, text_vbov);
        GLint pos_attrib = glGetAttribLocation(text_shader, "position");
        glEnableVertexAttribArray(pos_attrib);
        glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, text_vbouv);
        GLint uv_attrib = glGetAttribLocation(text_shader, "uv");
        glEnableVertexAttribArray(uv_attrib);
        glVertexAttribPointer(uv_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }
    glDrawArrays(GL_TRIANGLES, 0, tverts.size());
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
} // For drawing text on the gui, use -.95 .95 coord for good look
#endif