#pragma once

// #include <esp_now.h>
#include "ESP32_NOW.h"

#include <Arduino.h>
#include "gameEntities/game-entity.h"
#include "scenes/scene-manager.h"
#include "messages/message.h"

// Master
//const uint8_t L_MAC_1[] = {0x78, 0x21, 0x84, 0xDD, 0xF2, 0x84};
const uint8_t L_MAC_1[] = {0x7C, 0xDF, 0xA1, 0xE1, 0xBE, 0x60}; //S3_BOX HONZA 7c:df:a1:e1:be:60
// Slave
//const uint8_t L_MAC_2[] = {0x78, 0x21, 0x84, 0xDE, 0x08, 0x58};
const uint8_t L_MAC_2[] = {0x7C, 0xDF, 0xA1, 0xE8, 0x12, 0x58}; //S3_BOX VOJTA 7c:df:a1:e8:12:58

class NetworkManager : public ESP_NOW_Peer
{
private:
    static NetworkManager *instance;

    String myMAC;
    // ESP_NOW_Peer *peer;

    bool master;
    bool initialized;
    uint8_t broadcastAddress[6];
    GameEntity *gameEntity;
    SceneManager *sceneManager;

    template <typename T>
    void sendMessage(const T &message);

    //overriden methods
    void _onSent(bool success) override;
    void _onReceive(const uint8_t * data, size_t len) override;

    NetworkManager();
    // Pass nullptr for MAC address initially

public:
    ~NetworkManager();

    //get broadcastAddress
    const uint8_t * getBroadcastAddress() { return broadcastAddress; }
    bool isMaster() { return master; }
    bool isSlave() { return !master; }
    void startCommunication();
    Paddle *getMyPaddle() { return master ? gameEntity->getPaddle2() : gameEntity->getPaddle1(); };

    static NetworkManager *getInstance();
    static void initialize(GameEntity *gameEntity, SceneManager *sceneManager);
};