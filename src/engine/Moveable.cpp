#include "Moveable.hpp"

#include "../global.hpp"
#include "glm/gtx/vector_angle.hpp"

Moveable::Moveable(vec3 pos, float yaw, float pitch) : position(pos), yaw(yaw), pitch(pitch) {
  calcOrientation();
}

const float& Moveable::getSpeed()       const { return speed;       }
const float& Moveable::getYaw()         const { return yaw;         }
const float& Moveable::getPitch()       const { return pitch;       }
const vec3&  Moveable::getOrientation() const { return orientation; }
const vec3&  Moveable::getPosition()    const { return position;    }
const vec3&  Moveable::getUp()          const { return up;          }

vec3 Moveable::getBack()    const { return -getForward();};
vec3 Moveable::getLeft()    const { return -getRight(); }
vec3 Moveable::getRight()   const { return normalize(cross(up, -orientation));};
vec3 Moveable::getForward() const { return getOrientation();};
vec3 Moveable::getDown()    const { return -getUp();};

void Moveable::setSpeedDefault(float n)      { speedDefault = n; speed = speedDefault * speedMul; }
void Moveable::setSpeedMultiplier(float n)   { speedMul     = n; speed = speedDefault * speedMul; }
void Moveable::setPosition(const vec3& pos)  { position = pos; }

void Moveable::setView(const Moveable* rhs) {
  orientation = rhs->orientation;
  up = rhs->up;
  yaw = rhs->yaw;
  pitch = rhs->pitch;
}

void Moveable::moveForward() { position +=  orientation * speed * global::dt; }
void Moveable::moveBack()    { position += -orientation * speed * global::dt; }
void Moveable::moveLeft()    { position += getLeft()  * speed * global::dt; }
void Moveable::moveRight()   { position += getRight() * speed * global::dt; }
void Moveable::moveUp()      { position +=  up * speed * global::dt; }
void Moveable::moveDown()    { position += -up * speed * global::dt; }

void Moveable::moveByMouse(vec2 mousePos) {
  vec2 winSize = global::getWinSize();
  vec2 winCenter = winSize * 0.5f;
  vec2 pos = mousePos - winCenter * vec2(!global::guiFocused);

  vec2 delta = sensitivity * pos / winCenter;
  yaw += delta.x;
  pitch = std::clamp(pitch - delta.y, -PI_2 + 0.1f, PI_2 - 0.1f);

  calcOrientation();
}

void Moveable::calcOrientation() {
  vec3 o;
  o.x = cos(yaw) * cos(pitch);
  o.y = sin(pitch);
  o.z = sin(yaw) * cos(pitch);

  orientation = normalize(o);
}

