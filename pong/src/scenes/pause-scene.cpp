#include "pause-scene.h"
#include "images.cpp"
#include <Arduino.h>

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
    display->fillScreen(ST7789_BLACK);

    display->drawXBitmap(40, 20, espressif_logo_icon, 42, 42, ST7789_RED);
    display->drawXBitmap(40+42+20, 26, espressif_logo_name, 179, 30, ST7789_WHITE);
    
    display->setTextSize(3); // Draw 2X-scale text
    display->setTextColor(ST7789_WHITE);
    //display->setCursor(38, 12);
    display->setCursor(108, 110);
    display->println(F("PAUSED"));

    display->setTextSize(2);
    display->setTextColor(ST7789_WHITE);
    //display->setCursor(20, 56);
    display->setCursor(75, 190);
    
    display->println(F("Press to continue"));
  }
  if (drawPlayImage){
    display->fillTriangle(146, 130, 146, 162, 178, 146, ST7789_WHITE);
  } else {
    display->fillTriangle(146, 130, 146, 162, 178, 146, ST7789_BLACK);
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
