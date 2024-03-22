#include <Arduino.h>
#include "src/game-engine.h"

GameEngine* gameEngine;

void setup()
{
  Serial.begin(115200);
  
  delay(2000);  //wait for USB reconnection

  Serial.println("Arduino Multiplayer Pong");

  gameEngine = new GameEngine();
  gameEngine->start();

}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(10000));
}