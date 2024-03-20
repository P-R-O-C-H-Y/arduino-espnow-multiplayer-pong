#include "game-entity.h"
#include <Arduino.h>

GameEntity::GameEntity()
{
}

GameEntity::~GameEntity()
{
}

void GameEntity::initialize(DisplayProperties *displayProperties)
{
    this->displayProperties = displayProperties;
}

void GameEntity::resetGame()
{
    const int space = 12;
    int width = displayProperties->width;
    int height = displayProperties->height;

    resetBall();
    paddle1.setPosition(displayProperties->topLeftX + 1 + space, (height+displayProperties->topLeftY - paddle1.getHeight()) / 2);
    paddle1.setLastPosition(displayProperties->topLeftX + 1 + space, (height+displayProperties->topLeftY - paddle1.getHeight()) / 2);
    paddle2.setPosition(displayProperties->topRightX - paddle2.getWidth() - space, (height+displayProperties->topLeftY - paddle2.getHeight()) / 2);
    paddle2.setLastPosition(displayProperties->topRightX - paddle2.getWidth() - space, (height+displayProperties->topLeftY - paddle2.getHeight()) / 2);
    paddle1.resetScore();
    paddle2.resetScore();
}

void GameEntity::resetBall()
{
    ball.setPosition((displayProperties->width - ball.getRadius()) / 2, (displayProperties->height+displayProperties->topLeftY - ball.getRadius()) / 2);
    ball.setLastPosition((displayProperties->width - ball.getRadius()) / 2, (displayProperties->height+displayProperties->topLeftY - ball.getRadius()) / 2);

    float velX = START_VELOCITY_X;
    if (ball.getVelocityX() < 0)
    {
        velX *= -1;
    }
    ball.updateVelocity(velX, START_VELOCITY_Y);
}

bool GameEntity::preparePointMessage(PointMessage *pointMessage)
{
    if(!paddle1.isScoreChanged() && !paddle2.isScoreChanged()) return false;
    
    pointMessage->paddle1Point = paddle1.getScore();
    pointMessage->paddle2Point = paddle2.getScore();
    return true;
}