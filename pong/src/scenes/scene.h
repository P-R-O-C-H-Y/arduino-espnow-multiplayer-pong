#pragma once

#include <Adafruit_GFX.h>
#include <ESP32_S3_Box_TFT.h>
#include "../display-properties.h"

enum SceneType
{
    GAME,
    SCORE,
    START,
    PAUSE
};

class Scene
{
protected:
    SceneType type;
    ESP32S3BOX_TFT *display;
    DisplayProperties *displayProperties;
    bool rendered;

public:
    Scene();
    virtual ~Scene();

    void initialize(ESP32S3BOX_TFT *display, DisplayProperties *displayProperties);
    virtual void render() = 0;
    virtual void tick(){};
    virtual bool useTick() { return false; }
    virtual bool renderOnce() { return rendered; }

    SceneType getSceneType() { return type; }
};
