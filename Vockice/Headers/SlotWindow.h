#ifndef SLOTWINDOW_H
#define SLOTWINDOW_H

#include <vector>
#include <string>
#include <GL/glew.h>

class SlotWindow {
private:
    std::vector<unsigned int> textures; // Teksture za svaki simbol
    unsigned int shaderProgram;         // Shader program
    unsigned int VAO, VBO;              // OpenGL objekti za mrežu
    float yOffset;                      // Offset za pomeranje kolone
    float spinSpeed;                    // Brzina rotacije
    bool isSpinning;                    // Da li se kolona trenutno vrti
    float xPosition;                    // Horizontalna pozicija kolone

    float spinTime;             // Vreme koliko traje rotacija
    float elapsedSpinTime;      // Proteklo vreme rotacije
    std::vector<int> randomOrder; // Nasumični redosled tekstura



    void setupMesh();    
    unsigned int createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    unsigned int loadTexture(const std::string& filePath);
    unsigned int compileShader(GLenum type, const std::string& source);

public:
    SlotWindow(float xPosition, const std::vector<std::string>& texturePaths, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    void warmUp();
    ~SlotWindow();

    void render(float deltaTime);
    void startSpin();             
    void generateRandomOrder();
    void update(float deltaTime);
    void stopSpin();              

    int getMiddleSymbol() const;

    bool getIsSpinning() const;

};



#endif
