#include "Headers/Lever.h"
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// Konstruktor
Lever::Lever(float startX, float startY, float leverWidth, float leverHeight, float endY,
    float ballCenterX, float ballCenterY, float ballRightX,
    const std::string& texturePath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
    : x(startX), y(startY), width(leverWidth), height(leverHeight), startY(startY), endY(endY), isDragging(false) {
    shaderProgram = createShader(vertexShaderPath, fragmentShaderPath);
    textureID = loadTexture(texturePath);
    setupMesh();

    // Koristimo normalizovane vrednosti
    this->ballCenterX = ballCenterX;  // Normalizovana X koordinata centra
    this->ballCenterY = ballCenterY;  // Normalizovana Y koordinata centra
    this->ballRadius = ballRightX - ballCenterX;  // Izračunaj radijus na osnovu normalizovanih vrednosti

    // Debug ispis za proveru
    std::cout << "Normalized Ball Center: (" << this->ballCenterX << ", " << this->ballCenterY
        << "), Radius: " << this->ballRadius << std::endl;
}

Lever::~Lever() {
    // Oslobodite OpenGL resurse ako je potrebno
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &textureID);
    glDeleteProgram(shaderProgram);

    std::cout << "Lever resources released" << std::endl; // Debug ispis (opcionalno)
}


unsigned int Lever::loadTexture(const std::string& filePath) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Obrni teksturu vertikalno
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

// Funkcija za kompajliranje šejdera
unsigned int Lever::compileShader(GLenum type, const std::string& source) {
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

// Kreiranje shader programa
unsigned int Lever::createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
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

// Postavljanje mreže za slot kolonu
void Lever::setupMesh() {
    float vertices[] = {
     -0.2f, 0.2f, 0.0f, 1.0f, // Gornji levi
     -0.2f, -0.2f, 0.0f, 0.0f, // Donji levi
      0.2f, -0.2f, 1.0f, 0.0f, // Donji desni
      0.2f, 0.2f, 1.0f, 1.0f   // Gornji desni
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

// Renderovanje ručice
void Lever::render() const {
    glUseProgram(shaderProgram);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_TEXTURE_2D);

    glBindVertexArray(VAO);

    // Ažurirajte sadržaj VBO sa trenutnom pozicijom
    float vertices[] = {
        x, y, 0.0f, 1.0f,                 // Gornji levi ugao
        x, y - height, 0.0f, 0.0f,       // Donji levi ugao
        x + width, y - height, 1.0f, 0.0f, // Donji desni ugao
        x + width, y, 1.0f, 1.0f          // Gornji desni ugao
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDisable(GL_TEXTURE_2D);
    glUseProgram(0);
}

void Lever::startDrag() {
    isDragging = true;
}

void Lever::stopDrag() {
    isDragging = false;
}

void Lever::updatePosition(float mouseY) {
    if (isDragging) {
        y = std::max(endY, std::min(startY, mouseY));
    }
}

bool Lever::isBallClicked(float mouseX, float mouseY, int windowWidth, int windowHeight) const {
    float dx = mouseX - ballCenterX;
    float dy = mouseY - ballCenterY;
    float distanceSquared = dx * dx + dy * dy;

    

    return distanceSquared <= ballRadius * ballRadius;
}


void Lever::reset() {
    y = startY;
    isDragging = false;
}

bool Lever::isAtBottom() const {
    return y <= endY;
}

