#ifndef LEVER_HPP
#define LEVER_HPP

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Lever {
private:


    float ballCenterX;  // X koordinata centra loptice
    float ballCenterY;  // Y koordinata centra loptice
    float ballRadius;   // Radijus loptice


    float x, y;           // Trenutna pozicija ručice
    float width, height;  // Dimenzije ručice
    float startY;         // Početni Y položaj
    float endY;           // Krajnji Y položaj
    GLuint textureID;     // ID teksture ručice
    GLuint VAO, VBO;      // OpenGL objekti za mrežu
    GLuint shaderProgram; // OpenGL šejder program

    void setupMesh();
    unsigned int loadTexture(const std::string& texturePath);
    unsigned int compileShader(GLenum type, const std::string& source);
    unsigned int createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

public:
    Lever(float startX, float startY, float leverWidth, float leverHeight, float endY,
        float ballCenterX, float ballCenterY, float ballRightX,
        const std::string& texturePath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);    ~Lever();
    bool isDragging;      
    void startDrag();
    void stopDrag();
    void updatePosition(float mouseY);
    bool isBallClicked(float mouseX, float mouseY, int windowWidth, int windowHeight) const;
    void render() const;
    void reset();
    bool isClicked(float mouseX, float mouseY) const;
    bool isAtBottom() const;

};

#endif // LEVER_HPP
