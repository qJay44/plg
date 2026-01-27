#pragma once

#include "Moveable.hpp"

class Camera : public Moveable {
public:
  friend struct gui;

  Camera() = delete;
  Camera(Camera&) = delete;
  Camera(Camera&&) = delete;
  Camera(vec3 pos, float yaw, float pitch);
  virtual ~Camera() = default;

  const float& getNearPlane() const;
  const float& getFarPlane()  const;
  const float& getFov()       const;
  const mat4&  getProj()      const;
  const mat4&  getView()      const;
  const mat4&  getProjView()  const;

  void setNearPlane(const float& p);
  void setFarPlane(const float& p);
  void setFov(float f);

  void update();

protected:
  float nearPlane = 0.1f;
  float farPlane = 100.f;
  float fov = 45.f;
  float aspectRatio = 1.f;

  mat4 proj = mat4(1.f);
  mat4 view = mat4(1.f);
  mat4 pv   = mat4(1.f);
};

