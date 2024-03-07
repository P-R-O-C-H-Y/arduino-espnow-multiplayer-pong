#pragma once

#include "message.h"

enum EntityType { BALL, PADDLE };

struct PositionMessage : public Message
{
    EntityType enityType;
    unsigned int positionX;
    unsigned int positionY;
    unsigned int lastPositionX;
    unsigned int lastPositionY;

    PositionMessage()
    {
        messageType = POSITION;
    }
};
