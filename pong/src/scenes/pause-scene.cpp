#include "pause-scene.h"
#include <Arduino.h>

/* 32 x 32*/
static const unsigned char PAUSE_ICON[] = {0x0C, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00,
                                           0xFC, 0x03, 0x00, 0x00, 0xFC, 0x0F, 0x00, 0x00, 0xFC, 0x3F, 0x00, 0x00,
                                           0xFC, 0x7F, 0x00, 0x00, 0xFC, 0xFF, 0x01, 0x00, 0xFC, 0xFF, 0x07, 0x00,
                                           0xFC, 0xFF, 0x0F, 0x00, 0xFC, 0xFF, 0x3F, 0x00, 0xFC, 0xFF, 0xFF, 0x00,
                                           0xFC, 0xFF, 0xFF, 0x03, 0xFC, 0xFF, 0xFF, 0x07, 0xFC, 0xFF, 0xFF, 0x1F,
                                           0xFC, 0xFF, 0xFF, 0x3F, 0xFC, 0xFF, 0xFF, 0x3F, 0xFC, 0xFF, 0xFF, 0x1F,
                                           0xFC, 0xFF, 0xFF, 0x07, 0xFC, 0xFF, 0xFF, 0x03, 0xFC, 0xFF, 0xFF, 0x00,
                                           0xFC, 0xFF, 0x3F, 0x00, 0xFC, 0xFF, 0x0F, 0x00, 0xFC, 0xFF, 0x07, 0x00,
                                           0xFC, 0xFF, 0x01, 0x00, 0xFC, 0xFF, 0x00, 0x00, 0xFC, 0x1F, 0x00, 0x00,
                                           0xFC, 0x0F, 0x00, 0x00, 0xFC, 0x03, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00,
                                           0x3C, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00};

PauseScene::PauseScene()
{
  type = PAUSE;
}

PauseScene::~PauseScene()
{
}

void PauseScene::tick()
{
  if (millis() > lastTime + 500)
  {
    lastTime = millis();
    drawPlayImage = !drawPlayImage;
  }
}

void PauseScene::render()
{
  if(!rendered){
    //display->fillScreen(ST7789_BLACK);
    display->setTextSize(3); // Draw 2X-scale text
    display->setTextColor(ST7789_WHITE);
    //display->setCursor(38, 12);
    display->setCursor(116, 90);
    display->println(F("PAUSE"));

    display->setTextSize(2);
    display->setTextColor(ST7789_WHITE);
    //display->setCursor(20, 56);
    display->setCursor(75, 170);
    display->println(F("Press to continue"));
  }
  if (drawPlayImage){
    display->fillTriangle(146, 110, 146, 142, 178, 126, ST7789_WHITE);
  } else {
    display->fillTriangle(146, 110, 146, 142, 178, 126, ST7789_BLACK);
  }
  
  Scene::render();
  /*
      char strBuf[20]; // used for string formatting

      // Display the "You Lose" screen
      display->setFont(u8g_font_gdb20r); // switch to bigger font
      strcpy(strBuf, "PAUSE");
      display->drawStr((displayProperties->width - display->getStrWidth(strBuf)) / 2, displayProperties->height  / 2, strBuf);
      display->setFont(u8g_font_gdb11r); // switch to bigger font
      strcpy(strBuf, "Press to restart");
      display->drawStr((displayProperties->width - display->getStrWidth(strBuf)) / 2, displayProperties->height, strBuf);
      display->setFont(u8g_font_profont10r); // switch back to our normal font
      */
}
