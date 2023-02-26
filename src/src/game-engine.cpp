#include <Arduino.h>
#include "gameEntities/game-entity.h"
#include "game-engine.h"
#include "game-task-manager.h"

void xTaskRender(void *params)
{
    Serial.println(F("Starting task 'xTaskRender'"));
    GameEngine *engine = static_cast<GameEngine *>(params);
    engine->getRenderEngine()->render();
}

void xTaskGameLoop(void *params)
{
    Serial.println(F("Starting task 'xTaskGameLoop'"));
    GameEngine *engine = static_cast<GameEngine *>(params);
    engine->getGameLoopHandler()->update(engine->getGameEntity());
}

void xTaskInputManager(void *params)
{
    Serial.println(F("Starting task 'xTaskInputManager'"));
    GameEngine *engine = static_cast<GameEngine *>(params);
    engine->getInputManager()->startReading(engine->getGameEntity());
}

void xTaskNetwork(void *params)
{
    Serial.println(F("Starting task 'xTaskNetwork'"));
    GameTaskManagerTest* tasks = &GameTaskManager::getInstance()->tasks;

    while (true)
    {
        Serial.printf("Status of %s: %i\n", pcTaskGetName(tasks->inputTaskHandler), eTaskGetState(tasks->inputTaskHandler));
        Serial.printf("Status of %s: %i\n", pcTaskGetName(tasks->gameLoopTaskHandler), eTaskGetState(tasks->gameLoopTaskHandler));
        //Serial.printf("Status of %s: %i\n", pcTaskGetName(tasks->networkTaskHandler), eTaskGetState(tasks->inputTaskHandler));
        Serial.printf("Status of %s: %i\n", pcTaskGetName(tasks->renderTaskHandler), eTaskGetState(tasks->renderTaskHandler));

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

GameEngine::GameEngine() : running(false), sceneManager(&gameEntity, &renderEngine, &gameLoop), inputManager(&sceneManager)
{
    RenderEngine *renderEngine = getRenderEngine();
    gameLoop.setDisplayProperties(renderEngine->getDisplayProperties());
    gameEntity.initialize(renderEngine->getDisplayProperties());
}

GameEngine::~GameEngine()
{
}

void GameEngine::start()
{
    if (running)
        return;

    running = true;

    // TODO: network.init()
    createTasks();
    sceneManager.changeScene(START);
}

void GameEngine::createTasks()
{

    // Create the task for the rendering
    xTaskCreatePinnedToCore(
        xTaskRender, // Pointer to the task function
        "Render",    // Task name
        4096,        // Stack size in words
        this,        // Task parameter
        1,           // Task priority
        &GameTaskManager::getInstance()->tasks.renderTaskHandler,
        1);

    // Serial.println(eTaskGetState(GameTaskManager::getInstance()->getRenderTaskHandler()));

    // Create the task for input management
    xTaskCreatePinnedToCore(
        xTaskInputManager, // Pointer to the task function
        "InputMgr",        // Task name
        4096,              // Stack size in words
        this,              // Task parameter
        1,                 // Task priority
        &GameTaskManager::getInstance()->tasks.inputTaskHandler,
        1);

    // Create the task for the game loop
    xTaskCreatePinnedToCore(
        xTaskGameLoop, // Pointer to the task function
        "GameLoop",    // Task name
        4096,          // Stack size in words
        this,          // Task parameter
        2,             // Task priority
        &GameTaskManager::getInstance()->tasks.gameLoopTaskHandler,
        1);

    // Create the task for the game loop
    xTaskCreatePinnedToCore(
        xTaskNetwork, // Pointer to the task function
        "NetLoop",    // Task name
        4096,         // Stack size in words
        this,         // Task parameter
        1,            // Task priority
        &GameTaskManager::getInstance()->tasks.networkTaskHandler,
        1);
}

void GameEngine::stop()
{
    running = false;
}