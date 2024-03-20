#include "network-manager.h"
#include "WiFi.h"
#include "messages/position-message.h"
#include "messages/point-message.h"
#include "messages/scene-message.h"
#include "game-task-manager.h"

#include <esp_now.h>

NetworkManager *NetworkManager::instance = nullptr;

NetworkManager::NetworkManager() : ESP_NOW_Peer(nullptr), initialized(false)
{
    // ESP32 in Station Mode
    WiFi.mode(WIFI_STA);
    uint64_t mac = ESP.getEfuseMac();
    uint8_t macArray[6];
    for (int i = 0; i < 6; i++)
    {
        macArray[5-i] = (mac >> ((5 - i) * 8)) & 0xFF;
    }

    if (memcmp(macArray, L_MAC_1, 6) == 0)
    {
        Serial.println("\nI'm the Master\n");
        master = true;
        auto init = std::initializer_list<uint8_t>({L_MAC_2[0], L_MAC_2[1], L_MAC_2[2], L_MAC_2[3], L_MAC_2[4], L_MAC_2[5]});
        std::copy(init.begin(), init.end(), broadcastAddress);
    }
    else if (memcmp(macArray, L_MAC_2, 6) == 0)
    {
        Serial.println("\nI'm the Slave\n");
        master = false;
        auto init = std::initializer_list<uint8_t>({L_MAC_1[0], L_MAC_1[1], L_MAC_1[2], L_MAC_1[3], L_MAC_1[4], L_MAC_1[5]});
        std::copy(init.begin(), init.end(), broadcastAddress);
    }
    else
    {
        Serial.println("\nUnknow MAC address...\n");
    }

    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    WiFi.disconnect();
}

NetworkManager::~NetworkManager()
{
}

template <typename T>
void NetworkManager::sendMessage(const T &message)
{
    // Send message
    // esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&message, sizeof(T));
    if(this->send((uint8_t *)&message, sizeof(T)) == 0){
        Serial.println("Error sending message");
    }
    // Print results to serial monitor
    // checkResult(result, "Message sent!!");
}

void NetworkManager::startCommunication()
{
    Ball *ball = gameEntity->getBall();
    /*
    Paddle *paddle1 = gameEntity->getPaddle1();
    Paddle *paddle2 = gameEntity->getPaddle2();
    */

    Paddle *paddle = getMyPaddle();

    PositionMessage positionMessage;
    PointMessage pointMessage;
    SceneMessage sm;
    Message *pxRxedMessage;

    QueueHandle_t xQueue = GameTaskManager::getInstance()->tasks.networkQueueHandler;

    while (true)
    {
        if (!initialized)
        {
            Serial.println("Inizialization failed!");
            vTaskSuspend(NULL);
            continue;
        }

        if (master)
        {
            if(ball->preparePositionMessage(&positionMessage))
                sendMessage(positionMessage);

            if(gameEntity->preparePointMessage(&pointMessage))
                sendMessage(pointMessage);
        }

        if(paddle->preparePositionMessage(&positionMessage))
            sendMessage(positionMessage);

        if (xQueueReceive(xQueue, &(pxRxedMessage), 10) == pdPASS)
        {
            //Serial.printf("Received message from queue - messageType %i, queue space %i\n", pxRxedMessage->messageType, uxQueueSpacesAvailable(xQueue));
            if (pxRxedMessage->messageType == SCENE)
            {
                sm = *((SceneMessage *)pxRxedMessage);
                Serial.printf("pxRxedMessage: %p, sm: %p, st: %i\n", pxRxedMessage, &sm, sm.sceneType);
                sendMessage(sm);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(4));
    }
}

NetworkManager *NetworkManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new NetworkManager();
    }
    return instance;
}

void NetworkManager::initialize(GameEntity *gameEntity, SceneManager *sceneManager)
{
    log_i("NetworkManager initialize");
    instance = getInstance();

    if (instance->initialized){
        log_i("Already initialized");
        return;
    }

    // Deinit ESP-NOW to ensure a clean start
    //TODO: Check if this is necessary
    //esp_now_deinit();
    
    // Init ESP-NOW
    if (!ESP_NOW.begin())
    {
        log_e("Error initializing ESP-NOW");
        while(true);
    }
    log_i("ESP NOW INIT");

    // Once ESPNow is successfully Init, we will register for Send CB to get the status of Trasnmitted paket
    //esp_now_register_send_cb(onDataSent);

    log_i("Setting mac address and adding peer");

    //Set peer address
    if(!instance->addr(instance->broadcastAddress)){
        log_e("Error setting address");
        while(true);
    }
    //Set peer Interface
    if(!instance->setInterface(WIFI_IF_STA)){
        log_e("Error setting interface");
        while(true);
    }
    if (!instance->add()){
        log_e("Error adding peer");
        while(true);
    }

    //  Register for a callback function that will be called when data is received
     
    esp_now_rate_config_t cfg = {
        .phymode    = WIFI_PHY_MODE_HT20,
        .rate       = WIFI_PHY_RATE_MCS6_SGI,
        .ersu       = false
    };

    if(esp_now_set_peer_rate_config(instance->broadcastAddress, &cfg) != ESP_OK){
        log_e("Error setting peer rate config");
        vTaskDelay(pdMS_TO_TICKS(1000));
        //ESP.restart();
        while(true);
        return;
    }

    log_i("end of initialize");

    instance->sceneManager = sceneManager;
    instance->gameEntity = gameEntity;
    instance->initialized = true;
}

void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength)
// Formats MAC Address
{
    snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

// Callback when data is sent
void NetworkManager::_onSent(bool success){
    Serial.println(success ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void NetworkManager::_onReceive(const uint8_t * data, size_t len)
{
    Message receive_Data;
    memcpy(&receive_Data, data, sizeof(receive_Data));
    //Serial.printf("\nReceive Data - bytes received: %i\n", len);
    switch (receive_Data.messageType)
    {
    case POSITION:
    {
        PositionMessage pm;
        memcpy(&pm, data, sizeof(pm));
        unsigned int x = pm.positionX;
        unsigned int y = pm.positionY;
        unsigned int lastx = pm.lastPositionX;
        unsigned int lasty = pm.lastPositionY;
        //Serial.printf("Receive Data - entityType: [POSITION]\n positionX: %i\n positionY: %i\n lastPositionX: %i\n lastPositionY: %i\n", x, y, lastx, lasty);
        if (pm.enityType == BALL)
        {
            instance->gameEntity->getBall()->setPosition(x, y);
            instance->gameEntity->getBall()->setLastPosition(lastx, lasty);
        }
        else
        {
            if (instance->master){
                instance->gameEntity->getPaddle1()->setPosition(x, y);
                // instance->gameEntity->getPaddle1()->setLastPosition(lastx, lasty);
            } else {
                instance->gameEntity->getPaddle2()->setPosition(x, y);
                // instance->gameEntity->getPaddle2()->setLastPosition(lastx, lasty);
            }
        }
    }
    break;
    case POINT:
    {
        PointMessage sm;
        memcpy(&sm, data, sizeof(sm));
        instance->gameEntity->getPaddle1()->setPoint(sm.paddle1Point);
        instance->gameEntity->getPaddle2()->setPoint(sm.paddle2Point);
    }
    break;
    case SCENE:
    {
        SceneMessage sm;
        memcpy(&sm, data, sizeof(sm));
        Serial.printf("Receive Data - messageType: [SCENE], scenetype: %i\n", sm.sceneType);
        instance->sceneManager->changeScene(sm.sceneType);
    }
    break;
    }
}
