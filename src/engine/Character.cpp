#include "Character.hpp"

#include "Moveable.hpp"
#include "terrain/Terrain.hpp"
#include "Camera.hpp"
#include "../global.hpp"

Character::Character(Camera* camPtr, Terrain* terrainPtr)
  : Moveable(*camPtr),
    cam(camPtr), terrain(terrainPtr) {
  update();
}

void Character::moveForward() { position += naturalForward * speed * global::dt; }
void Character::moveBack()    { position -= naturalForward * speed * global::dt; }

void Character::moveUp() {
  velocity.y += jumpStrength * isOnGround() * !flyMode;
  position.y += jumpStrength * speedMul * flyMode * global::dt;
}

bool Character::isOnGround() const {
  return position.y == 0.f;
}

void Character::update() {
  using global::dt;
  naturalForward = normalize(cross(up, getRight()));
  velocity.y += flyMode ? 0.f : g * dt;

  position += velocity * dt;

  float terrainHeight = terrain->getHeightAt(position);
  float nextHeight = position.y - terrainHeight;

  if (nextHeight < 0.f) {
    position.y = terrainHeight;
    velocity.y = 0.f;
  }

  cam->setPosition(position + camOffset);
  cam->setView(this);
}

