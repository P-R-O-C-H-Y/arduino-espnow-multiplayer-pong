#include <Arduino.h>
#include "render-engine.h"
#include "gameEntities/game-entity.h"
#include "game-task-manager.h"

RenderEngine::RenderEngine() : display(), currentScene(nullptr)
{
    initDisplayProperties();
}

RenderEngine::~RenderEngine()
{
}

void RenderEngine::initDisplayProperties()
{
    // initialize the OLED object
    display.init();
    
    // clear the display
    //display.clearDisplay();
    display.setRotation(0);
    display.fillScreen(ST7789_BLACK);

    displayProperties = new DisplayProperties;
    displayProperties->width = SCREEN_WIDTH;
    displayProperties->height = SCREEN_HEIGHT;

    displayProperties->topLeftX = 0;
    displayProperties->topLeftY = 30;

    displayProperties->topRightX = SCREEN_WIDTH - 1;
    displayProperties->topRightY = 30;

    displayProperties->bottomLeftX = 0;
    displayProperties->bottomLeftY = SCREEN_HEIGHT - 1;

    displayProperties->bottomRightX = SCREEN_WIDTH - 1;
    displayProperties->bottomRightY = SCREEN_HEIGHT - 1;
}

void RenderEngine::render()
{
    int fps = 0;
    unsigned long lastTime = millis();

    while (true)
    {
        fps++;
        if (millis() > lastTime + 1000)
        {
            log_i("FPS: %i", fps);
            lastTime = millis();
            fps = 0;
        }
        if(currentScene != nullptr && currentScene->getSceneType() == GAME){
            delay(1000 / 200); // cap at 200 fps - game real fps 80 (ideal)
        }
        else{
            delay(1000 / 30); // cap at 60 fps
        }
        //display.fillScreen(ST7789_BLACK);
        if (currentScene != nullptr)
            currentScene->render();
        //display.display();

        /*TaskHandle_t myTaskHandle = xTaskGetCurrentTaskHandle();
        TaskHandle_t pointerHandle = GameTaskManager::getInstance()->tasks.renderTaskHandler;

        Serial.printf("MyTaskHandle status: %i, pointerHandle status: %i\n", eTaskGetState(myTaskHandle), eTaskGetState(pointerHandle));
        Serial.printf("MyTaskHandle pcTaskGetName: %s, pointerHandle pcTaskGetName: %s\n", pcTaskGetName(myTaskHandle), pcTaskGetName(pointerHandle));
        */

        if (currentScene != nullptr && currentScene->renderOnce())
        {
            Serial.printf("Try to suspend %s, from status %s\n", pcTaskGetName(GameTaskManager::getInstance()->tasks.renderTaskHandler), STATE_NAMES[eTaskGetState(GameTaskManager::getInstance()->tasks.renderTaskHandler)]);
            vTaskSuspend(GameTaskManager::getInstance()->tasks.renderTaskHandler);
        }
    }
}

void RenderEngine::changeScene(Scene *scene)
{
    Serial.printf("Change scene RenderEngine: %i -- %p\n", scene->getSceneType(), scene);


    scene->initialize(&display, displayProperties);
    currentScene = scene;

    display.fillScreen(ST7789_BLACK);
    Serial.printf("Try to resume %s, from status %s\n", pcTaskGetName(GameTaskManager::getInstance()->tasks.renderTaskHandler), STATE_NAMES[eTaskGetState(GameTaskManager::getInstance()->tasks.renderTaskHandler)]);

    vTaskResume(GameTaskManager::getInstance()->tasks.renderTaskHandler);
}