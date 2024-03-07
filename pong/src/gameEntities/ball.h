#pragma once

#include "paddle.h"
#include "entity.h"

#define START_VELOCITY_X    2//1
#define START_VELOCITY_Y    1.6f//0.8f
#define MIN_VELOCITY_X      1.2f//0.6f
#define MAX_VELOCITY_X      2.4f//1.2f
#define MAX_VELOCITY_Y      6//3

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
    const unsigned int RADIUS = 4;

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