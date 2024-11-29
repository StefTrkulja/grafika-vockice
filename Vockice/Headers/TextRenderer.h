#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Character {
    GLuint TextureID;   // ID teksture za karakter
    glm::ivec2 Size;    // Dimenzije karaktera
    glm::ivec2 Bearing; // Offset od baze linije
    GLuint Advance;     // Offset do sledećeg karaktera
};

class TextRenderer {
public:
    TextRenderer(const std::string& fontPath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, int fontSize);

    void RenderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

    ~TextRenderer(); 

private:
    GLuint VAO, VBO;                          // OpenGL objekti za renderovanje
    GLuint shaderProgram;                     // Šejder program specifičan za ovaj renderer
    std::map<char, Character> Characters;     // Mapa ASCII karaktera

    // Privatne funkcije
    unsigned int compileShader(GLenum type, const std::string& source);
    unsigned int createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    void loadCharacters(const std::string& fontPath, int fontSize);
};

#endif
