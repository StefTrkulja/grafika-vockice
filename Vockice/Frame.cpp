#include "headers/Frame.h"
#include <iostream>
#include <fstream>
#include <sstream>

Frame::Frame(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    shaderProgram = createShader(vertexShaderPath, fragmentShaderPath);
    setupMesh();
}

Frame::~Frame() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void Frame::setupMesh() {
    float vertices[] = {
        // X      Y
        -0.7f,  0.7f,
        -0.7f, -0.7f,
         0.7f, -0.7f,

        -0.7f,  0.7f,
         0.7f, -0.7f,
         0.7f,  0.7f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

unsigned int Frame::compileShader(GLenum type, const std::string& source) {
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

unsigned int Frame::createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    std::ifstream vsFile(vertexShaderPath);
    std::ifstream fsFile(fragmentShaderPath);

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

void Frame::render() const {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glUseProgram(0);
}
