#pragma once

#include "Terrain.hpp"
#include "gl/Camera.hpp"

class Character {
public:
  Character(Camera* camPtr, Terrain* terrainPtr) : cam(camPtr), terrain(terrainPtr) {
    position = cam->getPosition() - camOffset;
  }

  void update() {
    position = cam->getPosition() - camOffset;

    float terrainHeight = terrain->getHeightAt(position);
    float heightDiff = position.y - terrainHeight;

    if (heightDiff < 1.f)
      position.y += -heightDiff;

    cam->setPosition(position + camOffset);
  }

private:
  friend struct gui;

  Camera* cam = nullptr;
  Terrain* terrain = nullptr;

  vec3 position;
  vec3 camOffset{0.f, 5.f, 0.f};
};

