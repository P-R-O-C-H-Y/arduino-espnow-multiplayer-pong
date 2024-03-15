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
    drawScore();

    if(scored){ //redraw playing area and paddles after score
        display->fillRect(displayProperties->topLeftX, displayProperties->topLeftY+1, displayProperties->width, displayProperties->height - displayProperties->topLeftY-2, ST7789_BLACK);
        scored = false;
    }

    drawBall();
    drawPaddle(gameEntities->getPaddle1());
    drawPaddle(gameEntities->getPaddle2());

    drawBorder();

    display->drawXBitmap(160, displayProperties->topLeftY, DOTTED_LINE, 1, 240 - displayProperties->topLeftY, ST7789_WHITE);

    drawHeader();
    Scene::render();
}

void GameScene::drawHeader()
{
    display->setTextSize(1,2);
    display->setFont(&Org_01);
    display->setTextColor(ST7789_WHITE, ST7789_BLACK);
    display->setCursor(95, 19);
    display->print("ESP32 MULTIPLAYER PONG");
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
    static int8_t last_p1_score = -1;
    static int8_t last_p2_score = -1;

    display->setTextSize(3);
    display->setFont(&Org_01);
    display->setTextColor(ST7789_WHITE, ST7789_BLACK);
    display->setCursor(8, 20);
    if(gameEntities->getPaddle1()->getScore() != last_p1_score){
        last_p1_score = gameEntities->getPaddle1()->getScore(); 
        display->fillRect(0, 0, 30, 30, ST7789_BLACK);
        scored = true;
    }
    display->print(gameEntities->getPaddle1()->getScore());

    display->setFont(&Org_01);
    display->setTextColor(ST7789_WHITE, ST7789_BLACK);
    if(gameEntities->getPaddle2()->getScore() == 1){ // number 1, shift X cursor few pixels to the right
        display->setCursor(displayProperties->width - 8, 20);
    } else {
        display->setCursor(displayProperties->width - 20, 20);
    }
    if(gameEntities->getPaddle2()->getScore() != last_p2_score){
        last_p2_score = gameEntities->getPaddle2()->getScore();
        display->fillRect((displayProperties->width) - 20, 0, 30, 30, ST7789_BLACK);
        scored = true;
    }
    display->print(gameEntities->getPaddle2()->getScore());
}

void GameScene::drawBall()
{
    unsigned int ballLastX = gameEntities->getBall()->getLastPositionX();
    unsigned int ballLastY = gameEntities->getBall()->getLastPositionY();
    unsigned int ballX = gameEntities->getBall()->getPositionX();
    unsigned int ballY = gameEntities->getBall()->getPositionY();
    unsigned int ballR = gameEntities->getBall()->getRadius();
    display->fillCircle(ballLastX, ballLastY, ballR+4, ST7789_BLACK);
    display->fillCircle(ballX, ballY, ballR, ST7789_WHITE);
    //log_e("paddleLastX: %d, paddleX: %d", ballLastX, ballX);
    //log_e("paddleLastY: %d, paddleY: %d", ballLastY, ballY);
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

    display->fillRect(paddleX, paddleY, paddleWidth, paddleHeight, ST7789_WHITE);

    if (paddleLastY > paddleY){ //paddle moved up
        display->fillRect(paddleX, paddleY + paddleHeight, paddleWidth, paddleLastY - paddleY, ST7789_BLACK);
    }
    else if (paddleLastY < paddleY){ //paddle moved down
        //log_e("paddleLastY: %d, paddleY: %d", paddleLastY, paddleY);
        display->fillRect(paddleLastX, paddleLastY, paddleWidth, paddleY - paddleLastY, ST7789_BLACK);
    }
    else {} //padle not moved, nothing to clear

    if(paddleX < displayProperties->width/2){ //paddle left (slave)
        if (NetworkManager::getInstance()->isMaster()){
            paddle->setLastPosition(paddleX, paddleY);
        }
    }
    else {   //paddle right (master)
        if (NetworkManager::getInstance()->isSlave()){
            paddle->setLastPosition(paddleX, paddleY);
        }
    }

}