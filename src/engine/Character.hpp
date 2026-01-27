#pragma once

#include "Moveable.hpp"
#include "terrain/Terrain.hpp"
#include "Camera.hpp"

class Character : public Moveable {
public:
  Character() = delete;
  Character(Camera* camPtr, Terrain* terrainPtr);

  bool isOnGround() const;

  void update();

private:
  friend struct gui;

  Camera* cam = nullptr;
  Terrain* terrain = nullptr;

  vec3 naturalForward{};
  vec3 camOffset{0.f, 5.f, 0.f};

  vec3 velocity{};
  float g = -30.f;
  float jumpStrength = 15.f;

  bool flyMode = true;

private:
  void moveForward() override;
  void moveBack() override;
  void moveUp() override;
};

