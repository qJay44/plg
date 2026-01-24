#pragma once

class Moveable {
public:
  Moveable() = default;
  Moveable(vec3 pos, float yaw, float pitch);

  const float& getSpeed()       const;
  const float& getYaw()         const;
  const float& getPitch()       const;
  const vec3&  getOrientation() const;
  const vec3&  getPosition()    const;
  const vec3&  getUp()          const;

  vec3 getBack()    const;
  vec3 getLeft()    const;
  vec3 getRight()   const;
  vec3 getForward() const;
  vec3 getDown()    const;

  virtual void setSpeedDefault(float n);
  virtual void setSpeedMultiplier(float n);
  virtual void setPosition(const vec3& pos);
  virtual void setView(const Moveable* rhs);

  virtual void moveForward();
  virtual void moveBack();
  virtual void moveLeft();
  virtual void moveRight();
  virtual void moveUp();
  virtual void moveDown();
  virtual void moveByMouse(vec2 mousePos);

protected:
  vec3 position{};
  vec3 orientation{};
  vec3 up{0.f, 1.f, 0.f};
  float sensitivity = 1.f;
  float speedDefault = 1.f;
  float speedMul = 5.f;
  float speed = speedDefault * speedMul;
  float yaw = -PI;
  float pitch = 0.f;

protected:
  virtual void calcOrientation();
};

