#include "Headers/MouseHandler.h"
#include <iostream>

MouseHandler::MouseHandler(Player& player) : player(player) {}

void MouseHandler::setRectangles(float insertX, float insertY, float insertW, float insertH,
    float leftX, float leftY, float leftW, float leftH,
    float rightX, float rightY, float rightW, float rightH) {
    insertMoneyX = insertX;
    insertMoneyY = insertY;
    insertMoneyWidth = insertW;
    insertMoneyHeight = insertH;

    leftBetX = leftX;
    leftBetY = leftY;
    leftBetWidth = leftW;
    leftBetHeight = leftH;

    rightBetX = rightX;
    rightBetY = rightY;
    rightBetWidth = rightW;
    rightBetHeight = rightH;
}

void MouseHandler::processMouseClick(double mouseX, double mouseY, int windowWidth, int windowHeight) {
    // Transformacija koordinata miša iz prozorskih u OpenGL prostor
    float normalizedX = (mouseX / windowWidth) * 2.0f - 1.0f;
    float normalizedY = -((mouseY / windowHeight) * 2.0f - 1.0f);

    // Proveri "Insert Money"
    if (normalizedX >= insertMoneyX && normalizedX <= insertMoneyX + insertMoneyWidth &&
        normalizedY <= insertMoneyY && normalizedY >= insertMoneyY - insertMoneyHeight) {
        player.addFunds(100); // Dodaj 100 kredita
        std::cout << "Inserted 100 credits! Balance: " << player.getBalance() << std::endl;
    }

    // Proveri levi pravougaonik (smanjenje beta)
    if (normalizedX >= leftBetX && normalizedX <= leftBetX + leftBetWidth &&
        normalizedY <= leftBetY && normalizedY >= leftBetY - leftBetHeight) {
        player.decreaseBet();
        std::cout << "Decreased current bet to: " << player.getCurrentBet() << std::endl;
    }

    // Proveri desni pravougaonik (povećanje beta)
    if (normalizedX >= rightBetX && normalizedX <= rightBetX + rightBetWidth &&
        normalizedY <= rightBetY && normalizedY >= rightBetY - rightBetHeight) {
        player.increaseBet();
        std::cout << "Increased current bet to: " << player.getCurrentBet() << std::endl;
    }

}

