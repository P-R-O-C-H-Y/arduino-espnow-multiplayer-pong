#pragma once

#include <esp_now.h>
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

class NetworkManager
{
private:
    static NetworkManager *instance;

    String myMAC;
    esp_now_peer_info_t peerInfo;
    bool master;
    bool initialized;
    uint8_t broadcastAddress[6];
    GameEntity *gameEntity;
    SceneManager *sceneManager;

    bool addPeer();
    bool checkResult(esp_err_t addStatus, String success_message);

    template <typename T>
    void sendMessage(const T &message);
    static void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void onDataRecv(const esp_now_recv_info_t * info, const uint8_t *incomingData, int len);

    NetworkManager();

public:
    ~NetworkManager();

    bool isMaster() { return master; }
    bool isSlave() { return !master; }
    void startCommunication();
    Paddle *getMyPaddle() { return master ? gameEntity->getPaddle2() : gameEntity->getPaddle1(); };

    static NetworkManager *getInstance();
    static void initialize(GameEntity *gameEntity, SceneManager *sceneManager);
};