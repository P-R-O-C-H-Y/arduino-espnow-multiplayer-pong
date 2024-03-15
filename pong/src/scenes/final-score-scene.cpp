#include "final-score-scene.h"
#include <Arduino.h>
#include "../network-manager.h"
// #include <Arduino_FreeRTOS.h>

FinalScoreScene::FinalScoreScene(/* args */)
{
    type = SCORE;
}

FinalScoreScene::~FinalScoreScene()
{
}

void FinalScoreScene::render()
{
    display->fillScreen(ST7789_BLACK);
    display->setTextSize(3); // Draw 2X-scale text
    display->setTextColor(ST7789_WHITE);

    if (NetworkManager::getInstance()->getMyPaddle()->won()){
        display->setCursor(95, 124);
        display->println(F("YOU WIN!"));
    } else {
        display->setCursor(80, 124);
        display->println(F("YOU LOSE!"));
    }
    Scene::render();
}