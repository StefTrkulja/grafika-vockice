#include "headers/SlotWindow.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "stb_image.h"
#include <random>

// Konstruktor
SlotWindow::SlotWindow(float xPosition, const std::vector<std::string>& texturePaths, const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
    : xPosition(xPosition), yOffset(0.0f), spinSpeed(0.0f), isSpinning(false) {
    // Kreiranje shader programa
    shaderProgram = createShader(vertexShaderPath, fragmentShaderPath);

    // Učitavanje svih tekstura
    for (const auto& path : texturePaths) {
        textures.push_back(loadTexture(path));
    }

    for (size_t i = 0; i < textures.size(); ++i) {
        randomOrder.push_back(i);
    }
    
    generateRandomOrder(); // Inicijalizacija nasumičnog redosleda
    warmUp();
    setupMesh();
}


void SlotWindow::warmUp() {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glUseProgram(0);
}


// Destruktor
SlotWindow::~SlotWindow() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(static_cast<GLsizei>(textures.size()), textures.data());
    glDeleteProgram(shaderProgram);
}

// Funkcija za učitavanje tekstura
unsigned int SlotWindow::loadTexture(const std::string& filePath) {
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
unsigned int SlotWindow::compileShader(GLenum type, const std::string& source) {
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
unsigned int SlotWindow::createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
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
void SlotWindow::setupMesh() {
    float vertices[] = {
        // X      Y      S    T
        -0.22f,  -0.4f,  0.0f, 1.0f,  // Levo gore
        -0.22f, -0.6f,  0.0f, 0.0f,  // Levo dole
         0.05f, -0.6f,  1.0f, 0.0f,  // Desno dole

        -0.22f,  -0.40f,  0.0f, 1.0f,  // Levo gore
         0.05f, -0.60f,  1.0f, 0.0f,  // Desno dole
         0.05f,  -0.40f,  1.0f, 1.0f   // Desno gore
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

// Renderovanje slot kolone
void SlotWindow::render(float deltaTime) {
    if (isSpinning) {
        yOffset += spinSpeed * deltaTime;
        if (yOffset >= 0.17f) {
            yOffset -= 0.36f;
        }
        generateRandomOrder();
    }

//    std::cout << "Current yOffset: " << yOffset << std::endl;

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);

    size_t textureCount = textures.size();
    float slotHeight = 0.3f; // Razmak između centara slika

    for (size_t i = 0; i < textures.size(); ++i) {
        int textureIndex = randomOrder[i % textures.size()];
        glBindTexture(GL_TEXTURE_2D, textures[textureIndex]);

        // Izračunaj poziciju simbola
        float yPosition = yOffset + (static_cast<int>(i) * 0.3f); // 0.3f je visina jednog slota

        // Ciklično pomeranje simbola van vidljivog opsega
        if (yPosition > 1.0f) yPosition -= textures.size() * 0.3f;
        if (yPosition < -1.0f) yPosition += textures.size() * 0.3f;

        // Prikaži samo simbole koji su vidljivi (3 u opsegu -0.5f do 0.5f)
        if (yPosition < 0.8f && yPosition > -0.8f) {
            GLint locXOffset = glGetUniformLocation(shaderProgram, "xOffset");
            GLint locYOffset = glGetUniformLocation(shaderProgram, "yOffset");

            if (locXOffset == -1 || locYOffset == -1) {
                std::cerr << "Uniform not found in shader!" << std::endl;
                continue;
            }

            glUniform1f(locXOffset, xPosition);
            glUniform1f(locYOffset, yPosition);

            glDrawArrays(GL_TRIANGLES, 0, 6);

           /* std::cout << "Rendering texture " << i
                << " at yPosition: " << yPosition
                << " (visible)" << std::endl;*/
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);
}




void SlotWindow::update(float deltaTime) {
    if (isSpinning) {
        yOffset += spinSpeed * deltaTime;

        // Resetuj yOffset kada pređe opseg
        if (yOffset >= 1.0f) {
            yOffset -= 1.0f;
        }
    }
}

// Start rotacije
void SlotWindow::startSpin() {
    isSpinning = true;
    spinSpeed = 5.0f;
    spinTime = 1.0f; // Rotacija traje 5 sekundi
    elapsedSpinTime = 0.0f;
    yOffset = 0.0f;
    generateRandomOrder();
}





void SlotWindow::generateRandomOrder() {
    randomOrder.clear();

    // Popuni randomOrder sa indeksima tekstura
    for (size_t i = 0; i < textures.size(); ++i) {
        randomOrder.push_back(i);
    }

    // Nasumični generator sa seed-om zasnovanim na vremenu
    std::random_device rd;
    std::mt19937 g(rd());

    // Nasumično permutuj redosled
    std::shuffle(randomOrder.begin(), randomOrder.end(), g);
}


// Stop rotacije
void SlotWindow::stopSpin() {
    isSpinning = false;
    spinSpeed = 0.0f;

    float slotHeight = 0.3f; // Visina jednog slota

    yOffset = 0.0f;
}


int SlotWindow::getMiddleSymbol() const {
	float middleY = yOffset + 0.5f;
	if (middleY > 1.0f) middleY -= 1.0f;

	int middleSymbol = static_cast<int>(middleY / 0.3f);
	return randomOrder[middleSymbol];

}

 


bool SlotWindow::getIsSpinning() const {
    return isSpinning;
}