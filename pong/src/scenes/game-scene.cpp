#include "game-scene.h"
#include <Arduino.h>
#include <Fonts/Org_01.h>
#include "../gameEntities/collision.h"
#include "../network-manager.h"
#include "../game-task-manager.h"

// https://xbm.jazzychad.net/

/* 1 x 64 */
static const unsigned char PROGMEM DOTTED_LINE[] = {
    0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe,
    0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe,
    0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe,
    0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe,
    0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe,
    0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe,
    0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe,
    0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe,
    0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe,
    0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe };

const int SPACE = 15;

GameScene::GameScene(SceneManager *sm, GameEntity *ge) : sceneManager(sm), gameEntities(ge)
{
    type = GAME;
    //display->fillScreen(ST7789_BLACK);
}

GameScene::~GameScene()
{
}

void GameScene::tick()
{
    Paddle *paddle1 = gameEntities->getPaddle1();
    if (NetworkManager::getInstance()->isSlave())
    {
        paddle1->move();
        paddle1->collideWithBoard(displayProperties);

        return;
    }

    // Serial.println("<--GameTick-->");
    Ball *ball = gameEntities->getBall();
    Paddle *paddle2 = gameEntities->getPaddle2();

    ball->move();
    paddle2->move();

    // moveUsingAI(paddle1, true);
    // moveUsingAI(paddle2, true);

    paddle2->collideWithBoard(displayProperties);

    ball->collideWithPaddle(paddle1);
    ball->collideWithPaddle(paddle2);

    Collision collision = ball->collideWithBoard(displayProperties);
    switch (collision)
    {
    case LEFT:
        addPoint(paddle2);
        display->fillRect(0, 1, 11, displayProperties->height-2, ST7789_BLACK);
        break;
    case RIGHT:
        addPoint(paddle1);
        display->fillRect(309, 1, 11, displayProperties->height-2, ST7789_BLACK);
        break;
    default:
        break;
    }
}


void GameScene::addPoint(Paddle *paddle)
{
    paddle->addPoint();
    gameEntities->resetBall();
    if (paddle->won())
    {
        sm.sceneType = SCORE;
        GameTaskManager::getInstance()->networkQueueSend(&sm);
        sceneManager->changeScene(SCORE);
    }
}

/*
void GameScene::moveUsingAI(Paddle *paddle, bool godMode)
{
    Ball* ball = gameEntities->getBall();

    if (godMode)
    {
        paddle->setVelocityY(ball->getVelocityY());
    }
    else
    {
        if (ball->getVelocityX() < 0 && (ball->getPositionX() - paddle->getPositionX()) < 70)
        {
            if (paddle->getVelocityY() == 0.0f)
                paddle->setVelocityY(0.5f);

            if (ball->getVelocityY() > 0 && paddle->getVelocityY() < 0 || ball->getVelocityY() < 0 && paddle->getVelocityY() > 0)
            {
                paddle->setVelocityY(paddle->getVelocityY() * -1);
            }
        }
        else
        {
            paddle->setVelocityY(0.0f);
        }
    }

    paddle->move();
}
*/
void GameScene::render()
{
    // Serial.println("GameRender");


    drawBall();
    drawPaddle(gameEntities->getPaddle1());
    drawPaddle(gameEntities->getPaddle2());

    drawBorder();
    drawScore();

    display->drawXBitmap(160, 0, DOTTED_LINE, 1, 240, ST7789_WHITE);

    Scene::render();
}

void GameScene::drawBorder()
{
    // top line
    display->drawFastHLine(displayProperties->topLeftX, displayProperties->topLeftY, displayProperties->width, ST7789_WHITE);
    // bottom line
    display->drawFastHLine(displayProperties->bottomLeftX, displayProperties->bottomLeftY, displayProperties->width, ST7789_WHITE);
}

void GameScene::drawScore()
{
    display->setTextSize(2);
    display->setFont(&Org_01);
    display->setTextColor(ST7789_WHITE, ST7789_BLACK);
    display->setCursor((displayProperties->width/2) - 16, 10);
    display->fillRect((displayProperties->width/2) - 16, 1, 12, 12, ST7789_BLACK);
    display->print(gameEntities->getPaddle1()->getScore());

    display->setFont(&Org_01);
    display->setTextColor(ST7789_WHITE, ST7789_BLACK);
    display->setCursor((displayProperties->width/2) + 6, 10);
    display->fillRect((displayProperties->width/2) + 6, 1, 12, 12, ST7789_BLACK);
    display->print(gameEntities->getPaddle2()->getScore());
}

void GameScene::drawBall()
{
    unsigned int ballLastX = gameEntities->getBall()->getLastPositionX();
    unsigned int ballLastY = gameEntities->getBall()->getLastPositionY();
    unsigned int ballX = gameEntities->getBall()->getPositionX();
    unsigned int ballY = gameEntities->getBall()->getPositionY();
    unsigned int ballR = gameEntities->getBall()->getRadius();
    display->fillCircle(ballLastX, ballLastY, ballR+3, ST7789_BLACK);
    display->fillCircle(ballX, ballY, ballR, ST7789_WHITE);
    // gameEntities->getBall()->setLastPosition(ballX, ballY);
}

void GameScene::drawPaddle(Paddle *paddle)
{
    int paddleLastX = paddle->getLastPositionX();
    int paddleLastY = paddle->getLastPositionY();
    int paddleX = paddle->getPositionX();
    int paddleY = paddle->getPositionY();
    int paddleWidth = paddle->getWidth();
    int paddleHeight = paddle->getHeight();

    //NEEDS TO BE UPDATED TO DRAW PADDLE WHEN SCREEN IS CHANGED


    //TODO: redraw paddle by checking Y change   
    //if (paddleLastX != paddleX || paddleLastY != paddleY){
    display->fillRect(paddleX, paddleY, paddleWidth, paddleHeight, ST7789_WHITE);
    //remove old paddle when moved up only few pixels no not overdraw
    if (paddleLastY > paddleY){ //paddle moved up
        display->fillRect(paddleX, paddleY + paddleHeight, paddleWidth, paddleLastY - paddleY, ST7789_BLACK);
    }
    else if (paddleLastY < paddleY){ //paddle moved down
        //log_e("paddleLastY: %d, paddleY: %d", paddleLastY, paddleY);
        display->fillRect(paddleLastX, paddleLastY, paddleWidth, paddleY - paddleLastY, ST7789_BLACK);
    }
    else {} //padle not moved

    if(paddleX < displayProperties->width/2){ //paddle left (slave)
        // display->fillRect(0, 1, paddleWidth+11, displayProperties->height-2, ST7789_BLACK);
        // display->fillRect(paddleX, paddleY, paddleWidth, paddleHeight, ST7789_WHITE);
        if (NetworkManager::getInstance()->isMaster()){
            paddle->setLastPosition(paddleX, paddleY);
        }
    }
    else {   //paddle right (master)
        // display->fillRect(305, 1, paddleWidth+11, displayProperties->height-2, ST7789_BLACK);
        // display->fillRect(paddleX, paddleY, paddleWidth, paddleHeight, ST7789_WHITE);
        if (NetworkManager::getInstance()->isSlave()){
            paddle->setLastPosition(paddleX, paddleY);
        }
    }

}