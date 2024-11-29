#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Light {
private:
    unsigned int VAO, VBO, textureID, shaderProgram;
    float intensity;
    glm::vec2 position; // Dodata pozicija

    void setupMesh();
    unsigned int loadTexture(const std::string& filePath);
    unsigned int compileShader(GLenum type, const std::string& source);
    unsigned int createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

public:
    Light(const std::string& texturePath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, glm::vec2 initPosition);
    ~Light();

    void render(float currentIntensity) const;
};

#endif
