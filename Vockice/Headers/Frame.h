#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <GL/glew.h>

class Frame {
public:
    Frame(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    ~Frame();
    void render() const;

private:
    unsigned int shaderProgram, VAO, VBO;

    unsigned int createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    unsigned int compileShader(GLenum type, const std::string& source);
    void setupMesh();
};

#endif
