#include <Arduino.h>
#include "render-engine.h"
#include "gameEntities/game-entity.h"

RenderEngine::RenderEngine() : display(U8G2_R0, /* reset=*/U8X8_PIN_NONE), currentScene(NULL)
{
    initDisplayProperties();
    clearDisplay();
}

RenderEngine::~RenderEngine()
{
}

void RenderEngine::initDisplayProperties()
{
    display.begin();
    int w = display.getWidth();
    int h = display.getHeight();

    displayProperties = new DisplayProperties;
    displayProperties->width = w;
    displayProperties->height = h;

    displayProperties->topLeftX = 0;
    displayProperties->topLeftY = 0;

    displayProperties->topRightX = w - 1;
    displayProperties->topRightY = 0;

    displayProperties->bottomLeftX = 0;
    displayProperties->bottomLeftY = h - 1;

    displayProperties->bottomRightX = w - 1;
    displayProperties->bottomRightY = h - 1;
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
            Serial.print("FPS: ");
            Serial.println(fps);
            lastTime = millis();
            fps = 0;
        }

        display.firstPage();
        do
        {
            if (currentScene != NULL)
                currentScene->render();
        } while (display.nextPage());
    }
}

void RenderEngine::changeScene(Scene *scene)
{
    scene->initialize(&display, displayProperties);
    currentScene = scene;
}

void RenderEngine::clearDisplay()
{
    // Clear the screen
    display.firstPage();
    do
    {
    } while (display.nextPage());
}