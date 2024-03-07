#include "scene.h"
#include <Arduino.h>

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::initialize(ESP32S3BOX_TFT *display, DisplayProperties *displayProperties)
{
    this->display = display;
    this->displayProperties = displayProperties;
    this->rendered = false;
}

void Scene::render()
{
    rendered = true;
}