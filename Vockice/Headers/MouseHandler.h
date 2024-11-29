#ifndef MOUSEHANDLER_H
#define MOUSEHANDLER_H

#include "Player.h"

class MouseHandler {
private:
    Player& player; 

    float insertMoneyX, insertMoneyY, insertMoneyWidth, insertMoneyHeight;
    float leftBetX, leftBetY, leftBetWidth, leftBetHeight;
    float rightBetX, rightBetY, rightBetWidth, rightBetHeight;
    float leverX, leverY, leverWidth, leverHeight;


public:
    MouseHandler(Player& player);

    void setRectangles(float insertX, float insertY, float insertW, float insertH,
        float leftX, float leftY, float leftW, float leftH,
        float rightX, float rightY, float rightW, float rightH);

    void setLeverRectangle(float x, float y, float width, float height); 



    // Metoda za obradu klikova miša
    void processMouseClick(double mouseX, double mouseY, int windowWidth, int windowHeight);
    bool isLeverClicked(double mouseX, double mouseY, int windowWidth, int windowHeight); 

};


#endif
