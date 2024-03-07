#pragma once

#include "gameEntities/game-entity.h"
#include "scenes/scene-manager.h"

class InputManager
{
private:
    const int HORZ = GPIO_NUM_9;
    const int VERT = GPIO_NUM_14;
    const int SEL = GPIO_NUM_40;
    SceneManager* sceneManager;

public:
    InputManager(SceneManager* sceneManager);
    ~InputManager();

    void startReading(GameEntity* gameEntities);
};
