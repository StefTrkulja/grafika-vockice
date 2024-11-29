#include "Headers/Column.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "stb_image.h"

Column::Column(float x, const std::string& texturePath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
    : xPosition(x), yOffset(0.0f), spinSpeed(0.0f), isSpinning(false) {
    // Kreiranje shader programa
    shaderProgram = createShader(vertexShaderPath, fragmentShaderPath);

    // Učitavanje teksture
    textureID = loadTexture(texturePath);

    // Postavljanje mreže
    setupMesh();
}

Column::~Column() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &textureID);
    glDeleteProgram(shaderProgram);
}

void Column::setupMesh() {
    float vertices[] = {
        // X      Y      S    T
        -0.2f,  0.4f,  0.0f, 1.0f,
        -0.2f, -0.4f,  0.0f, 0.0f,
         0.2f, -0.4f,  1.0f, 0.0f,

        -0.2f,  0.4f,  0.0f, 1.0f,
         0.2f, -0.4f,  1.0f, 0.0f,
         0.2f,  0.4f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

unsigned int Column::compileShader(GLenum type, const std::string& source) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
    }

    return shader;
}

unsigned int Column::createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    std::ifstream vsFile(vertexShaderPath);
    std::ifstream fsFile(fragmentShaderPath);

    if (!vsFile.is_open() || !fsFile.is_open()) {
        std::cerr << "Failed to load shader files: " << vertexShaderPath << ", " << fragmentShaderPath << std::endl;
        return 0;
    }

    std::stringstream vsStream, fsStream;
    vsStream << vsFile.rdbuf();
    fsStream << fsFile.rdbuf();

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vsStream.str());
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsStream.str());

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

unsigned int Column::loadTexture(const std::string& filePath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        return 0;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return texture;
}

void Column::startSpin() {
    isSpinning = true;
    spinSpeed = 2.0f;
}

void Column::stopSpin() {
    isSpinning = false;
    spinSpeed = 0.0f;
}

void Column::render(float deltaTime) {
    if (isSpinning) {
        yOffset += spinSpeed * deltaTime;
        if (yOffset >= 1.0f) {
            yOffset = 0.0f;
        }
    }

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLint locYOffset = glGetUniformLocation(shaderProgram, "yOffset");
    GLint locXPosition = glGetUniformLocation(shaderProgram, "xPosition");

    glUniform1f(locYOffset, yOffset);
    glUniform1f(locXPosition, xPosition);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}
