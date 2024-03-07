#pragma once

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <ESP32_S3_Box_TFT.h>
#include "display-properties.h"
#include "scenes/scene.h"

#define SCREEN_WIDTH 320 // OLED display width, in pixels
#define SCREEN_HEIGHT 240 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
//#define OLED_RESET -1 // Reset pin
//#define SCREEN_ADDRESS 0x3C

class RenderEngine
{
private:
    ESP32S3BOX_TFT display;
    DisplayProperties* displayProperties;
    Scene* currentScene;
    
    void initDisplayProperties();

public:
    RenderEngine();
    ~RenderEngine();
    
    DisplayProperties* getDisplayProperties() { return displayProperties; }
    void changeScene(Scene* scene);
    Scene* getCurrentScene(){ return currentScene;}
    void render();
};