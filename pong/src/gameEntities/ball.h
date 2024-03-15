#pragma once

#include "paddle.h"
#include "entity.h"

#define START_VELOCITY_X    3//1
#define START_VELOCITY_Y    2.4f//0.8f
#define MIN_VELOCITY_X      1.8f//0.6f
#define MAX_VELOCITY_X      3.6f//1.2f
#define MAX_VELOCITY_Y      9//3

struct Boundary
{
    float x;
    float y;
    float width;
    float height;
};

class Ball : public Entity
{
private:
    const unsigned int RADIUS = 3;

    void addRandomness(float randomX, float randomY);
    bool intersect(const Boundary &r1, const Boundary &r2);

public:
    Ball();
    ~Ball();

    unsigned int getRadius() { return RADIUS; }
    void collideWithPaddle(Paddle *paddle);
    virtual Collision collideWithBoard(DisplayProperties *displayProperties);
    bool preparePositionMessage(PositionMessage* positionMessage) override;

};