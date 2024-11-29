
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Headers/Background.h"
#include "Headers/Column.h"
#include "Headers/SlotWindow.h"
#include "Headers/Frame.h"
#include "Headers/MouseHandler.h"
#include "Headers/TextRenderer.h"
#include "Headers/Lever.h"
#include "Headers/Light.h"

Player player(1000);
MouseHandler mouseHandler(player);



float frameTime = 1.0f / 60.0f; 
float lastFrameTime = glfwGetTime();


void toggleFullscreen(GLFWwindow* window, bool& isFullscreen, int& windowedWidth, int& windowedHeight, int& windowedPosX, int& windowedPosY) {
    if (isFullscreen) {
        glfwSetWindowMonitor(window, nullptr, windowedPosX, windowedPosY, windowedWidth, windowedHeight, 0);
        isFullscreen = false;
    }
    else {
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
        glfwGetWindowPos(window, &windowedPosX, &windowedPosY);

        glfwSetWindowMonitor(window, primaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        isFullscreen = true;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
}


void getNormalizedMousePosition(GLFWwindow* window, double& normX, double& normY) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    normX = (mouseX / width) * 2.0 - 1.0;   
    normY = 1.0 - (mouseY / height) * 2.0;  
}


int main() {
    if (!glfwInit()) {
        std::cout << "GLFW nije mogao da se inicijalizuje! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    unsigned int wWidth = 1000, wHeight = 800;
    const char wTitle[] = "Kockarski San";
    GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, wTitle, nullptr, nullptr);

    if (window == nullptr) {
        std::cout << "Prozor nije mogao da se kreira! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int windowedWidth = wWidth, windowedHeight = wHeight;
    int windowedPosX, windowedPosY;
    bool isFullscreen = false;

    Background background("textures/bg3_final.jpg", "Shaders/slotwindow.vert", "Shaders/slotwindow.frag");
    Background slot_room_background("textures/slotroomv2.jpg", "Shaders/basic.vert", "Shaders/basic.frag");
    std::vector<std::string> textures = {
        "textures/s3.png",
        "textures/s4.png",
        "textures/s5.png",
        "textures/s6.png"
    };

    SlotWindow slot1(-0.5f, textures, "Shaders/slotwindow.vert", "Shaders/basic.frag");
    SlotWindow slot2(-0.185f, textures, "Shaders/slotwindow.vert", "Shaders/basic.frag");
    SlotWindow slot3(0.13f, textures, "Shaders/slotwindow.vert", "Shaders/basic.frag");


    Frame frame("Shaders/slotwindow.vert", "Shaders/basic.frag");

    float lastFrame = 0.0f;

    float spinStartTime = glfwGetTime();

    mouseHandler.setRectangles(
        0.310156f, -0.766667f, 0.325f, 0.095833f, 
        0.45f, -0.305556f, 0.057031f, 0.084722f,  
        0.544531f, -0.305556f, 0.057031f, 0.083333f 
    );


    TextRenderer textRenderer("fonts/arial.ttf", "Shaders/text.vert", "Shaders/text.frag", 36);
    TextRenderer textRenderIndeks("fonts/arial.ttf", "Shaders/text.vert", "Shaders/text.frag", 28);
    Light light1("textures/vivalageng.png", "Shaders/light.vert", "Shaders/light.frag", glm::vec2(-0.58f, 0.005f));
    Light light2("textures/vivalageng.png", "Shaders/light.vert", "Shaders/light.frag", glm::vec2(-0.265f, 0.005f));
    Light light3("textures/vivalageng.png", "Shaders/light.vert", "Shaders/light.frag", glm::vec2(0.05f, 0.005));


    glm::vec2 lightPositions[3] = {
    glm::vec2(-0.5f, -0.5f),  
    glm::vec2(0.0f, -0.5f),   
    glm::vec2(0.5f, -0.5f)    
    };

   
    Lever lever(
        0.71f,  // X pozicija
        0.55f,  // Početna Y pozicija
        0.22f,  // Širina (povećano)
        0.5f,  // Visina (povećano)
        0.07f,
        0.822f,  // Normalizovana X koordinata centra loptice
        0.435f,  // Normalizovana Y koordinata centra loptice
        0.92f,   // Normalizovana X koordinata desne ivice loptice
        "textures/lvr.png",
        "shaders/basic.vert",
        "shaders/basic.frag"
    );
    bool gameStarted = false;
    bool mouseHandled = false;
    bool isGameOver = false;
    bool isBigWinActive = false;       
    float bigWinTimer = 5.0f;          
    float bigWinDuration = 5.0f;
    std::string currentMessage = "";
    glm::vec3 messageColor = glm::vec3(1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrameTime;

        if (deltaTime < frameTime) {
            glfwWaitEventsTimeout(frameTime - deltaTime);
            currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrameTime;
        }
        lastFrameTime = currentFrame;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		slot_room_background.render();  
        background.render();
        
        lever.render();



        int windowedWidth = wWidth, windowedHeight = wHeight;
        int windowedPosX, windowedPosY;
        bool isFullscreen = false;

        static bool fullscreenHandled = false;
        if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
            if (!fullscreenHandled) {
                toggleFullscreen(window, isFullscreen, windowedWidth, windowedHeight, windowedPosX, windowedPosY);
                fullscreenHandled = true;
            }
        }
        else {
            fullscreenHandled = false;
        }



        if (!gameStarted) {
            textRenderer.RenderText("Stop while you can!", 300.0f, 660.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

       


        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (!mouseHandled) {
                mouseHandler.processMouseClick(mouseX, mouseY, width, height);
                mouseHandled = true; 
            }
        }
        else {
            mouseHandled = false;
        }


        double normMouseX, normMouseY;
        getNormalizedMousePosition(window, normMouseX, normMouseY);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (!lever.isDragging) {
                if (lever.isBallClicked(normMouseX, normMouseY, width, height)) {
                    lever.startDrag(); 
                }
            }
            if (lever.isDragging) {
                lever.updatePosition(normMouseY); 
            }
        }
        else if (lever.isDragging) {
            lever.stopDrag(); 
        }

        if (lever.isAtBottom() && !isBigWinActive) {
            if (player.getBalance() >= player.getCurrentBet()) {
                player.placeBet();
                gameStarted = true;
                slot1.startSpin();
                slot2.startSpin();
                slot3.startSpin();
                lever.reset(); 
                spinStartTime = glfwGetTime();
            }
            else {
                currentMessage = "You don't have enough credits";
                messageColor = glm::vec3(1.0f, 0.0f, 0.0f); 
            }

        }


        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS &&
            !slot1.getIsSpinning() && !slot2.getIsSpinning() && !slot3.getIsSpinning() && !isBigWinActive) {
            if (player.getBalance() >= player.getCurrentBet()) {
                player.placeBet();
                gameStarted = true;
                currentMessage = "";
                slot1.startSpin();
                slot2.startSpin();
                slot3.startSpin();
                spinStartTime = glfwGetTime();
            }
            else {
                currentMessage = "You don't have enough credits";
                messageColor = glm::vec3(1.0f, 0.0f, 0.0f); 
            }
        }

        if (currentFrame - spinStartTime > 1.0f &&
            (slot1.getIsSpinning() || slot2.getIsSpinning() || slot3.getIsSpinning())) {
            slot1.stopSpin();
            slot2.stopSpin();
            slot3.stopSpin();

            int symbol1 = slot1.getMiddleSymbol();
            int symbol2 = slot2.getMiddleSymbol();
            int symbol3 = slot3.getMiddleSymbol();

            if (symbol1 == symbol2 && symbol2 == symbol3) {
                int winnings = player.getCurrentBet() * 10;
                player.addWinnings(winnings);
				player.recordSpin(winnings);    
                currentMessage = "Big Win! You earned " + std::to_string(winnings) + "$!";
                messageColor = glm::vec3(0.0f, 1.0f, 0.0f);
                isBigWinActive = true;         
                bigWinTimer = 0.0f;            
            }
            else {
                player.recordSpin(-player.getCurrentBet());
                currentMessage = "Try again! Better luck next time!";
                messageColor = glm::vec3(1.0f, 0.0f, 0.0f);
                if (player.getBalance() <= 0) {
                    isGameOver = true;
                }
            }
        }

        if (!currentMessage.empty()) {
            if (isBigWinActive) {
                bigWinTimer += deltaTime;

                if (bigWinTimer >= bigWinDuration) {
                    isBigWinActive = false; 
                    messageColor = glm::vec3(0.0f, 1.0f, 0.0f); 
                }
                else {
                    // Dinamičko menjanje boja pomoću sinusnih funkcija
                    float r = (sin(bigWinTimer * 3.0f) + 1.0f) / 2.0f; 
                    float g = (sin(bigWinTimer * 2.0f) + 1.0f) / 2.0f; 
                    float b = (sin(bigWinTimer * 4.0f) + 1.0f) / 2.0f; 
                    messageColor = glm::vec3(r, g, b);
                }
            }

            float xPos = 220.0f; 
            if (currentMessage.find("Sorry") != std::string::npos) {
                xPos = 240.0f; 
            }
            textRenderer.RenderText(currentMessage, xPos, 660.0f, 1.0f, messageColor);
        }

        if (isBigWinActive) {
            float timeValue = glfwGetTime();
            float lightIntensity = (sin(timeValue * 10.0f) + 1.0f) / 2.0f; 

            int activeLight = static_cast<int>(timeValue * 2) % 2; 

            if (activeLight == 0) {
                light1.render(lightIntensity);
                light3.render(lightIntensity);
            }
            else {
                light2.render(lightIntensity);
            }
        }


        slot1.update(deltaTime);
        slot2.update(deltaTime);
        slot3.update(deltaTime);

        slot1.render(deltaTime);
        slot2.render(deltaTime);
        slot3.render(deltaTime);

        textRenderer.RenderText(std::to_string(player.getBalance()), 703.0f, 495.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        textRenderer.RenderText(std::to_string(player.getCurrentBet()), 703.0f, 330.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        
        textRenderIndeks.RenderText("Stefan Trkulja", 810.0f, 760.0f, 1.0f, glm::vec3(1.0f, 0.0f, 1.0f));
        textRenderIndeks.RenderText("RA150/2021", 810.0f, 720.0f, 1.0f, glm::vec3(1.0f, 0.0f, 1.0f));


        if (isGameOver) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
            glClear(GL_COLOR_BUFFER_BIT);

            textRenderer.RenderText("Game Over", 300.0f, 500.0f, 1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
            textRenderer.RenderText("You lost money buddy!", 280.0f, 450.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

            textRenderer.RenderText("Game statistics :", 280.0f, 400.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            textRenderer.RenderText("Total number of spins: " + std::to_string(player.getTotalSpins()), 280.0f, 350.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
            textRenderer.RenderText("Total winnings: " + std::to_string(player.getTotalWinnings()), 280.0f, 320.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
            textRenderer.RenderText("Total losses: " + std::to_string(player.getTotalLosses()), 280.0f, 290.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
            textRenderer.RenderText("Highest win: " + std::to_string(player.getHighestWin()), 280.0f, 260.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));

            textRenderer.RenderText("Press [ESC] to exit the application", 200.0f, 150.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true); 
            }

            glfwSwapBuffers(window); 
            glfwPollEvents(); 
            continue; 
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
