#pragma once

#include "Moveable.hpp"
#include "terrain/Terrain.hpp"
#include "Camera.hpp"
#include "../global.hpp"

class Character : public Moveable {
public:
  Character(Camera* camPtr, Terrain* terrainPtr)
    : Moveable(camPtr->getPosition() - vec3{0.f, 5.f, 0.f}, camPtr->getYaw(), camPtr->getPitch()),
      cam(camPtr), terrain(terrainPtr) {
    speedDefault = 100.f;
  }

  void update() {
    position.y -= fallSpeed * fallAcc++ * global::dt;

    float terrainHeight = terrain->getHeightAt(position);
    float heightDiff = position.y - terrainHeight;

    if (heightDiff < 1.f) {
      position.y += -heightDiff;
      fallAcc = 1.f;
    }

    cam->setPosition(position + camOffset);
    cam->setView(this);

  }

private:
  friend struct gui;

  Camera* cam = nullptr;
  Terrain* terrain = nullptr;

  vec3 camOffset{0.f, 5.f, 0.f};

  float fallSpeed = 1.f;
  float fallAcc = 1.f;
};

