#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <string>
#include <GL/glew.h>

class Background {
public:
    Background(const std::string& texturePath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    ~Background();

    void render() const;

private:
    unsigned int VAO, VBO, textureID, shaderProgram;

    void setupMesh();
    unsigned int compileShader(GLenum type, const std::string& source);
    unsigned int createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    unsigned int loadTexture(const std::string& filePath);
};

#endif // BACKGROUND_H
