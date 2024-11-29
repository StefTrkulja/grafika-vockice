#ifndef COLUMN_H
#define COLUMN_H

#include <string>
#include <GL/glew.h>

class Column {
public:
    Column(float x, const std::string& texturePath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    ~Column();

    void startSpin();
    void stopSpin();
    void render(float deltaTime);

private:
    void setupMesh();
    unsigned int compileShader(GLenum type, const std::string& source);
    unsigned int createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    unsigned int loadTexture(const std::string& filePath);

    unsigned int VAO, VBO;
    unsigned int textureID;
    unsigned int shaderProgram;

    float xPosition;
    float yOffset;
    float spinSpeed;
    bool isSpinning;
};

#endif
