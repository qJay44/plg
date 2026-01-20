#pragma once

class Camera {
public:
  friend struct gui;

  Camera() = delete;
  Camera(Camera&) = delete;
  Camera(Camera&&) = delete;
  Camera(vec3 pos, vec3 orientation, double sensitivity);
  virtual ~Camera() = default;

  const float& getNearPlane()     const;
  const float& getFarPlane()      const;
  const float& getSpeed()         const;
  const float& getFov()           const;
  const vec3&  getOrientation()   const;
  const vec3&  getPosition()      const;
  const vec3&  getUp()            const;
  const mat4&  getMatrix()        const;
  const mat4&  getMatrixInverse() const;

  vec3 getBack()    const;
  vec3 getLeft()    const;
  vec3 getRight()   const;
  vec3 getForward() const;
  vec3 getDown()    const;

  void setNearPlane(const float& p);
  void setFarPlane(const float& p);
  void setSpeedDefault(const float& s);
  void setSpeedMultiplier(const float& m);
  void setFov(float f);

  void update();

  virtual void moveForward();
  virtual void moveBack();
  virtual void moveLeft();
  virtual void moveRight();
  virtual void moveUp();
  virtual void moveDown();
  virtual void moveByMouse(vec2 mousePos);

protected:
  vec3 position, orientation;
  float sensitivity;

  float nearPlane = 0.1f;
  float farPlane = 100.f;
  float fov = 45.f;

  vec3 up{0.f, 1.f, 0.f};
  float speedDefult = 5.f;
  float speedMul = 1.f;
  float speed = speedDefult * speedMul;

  mat4 view   = mat4(1.f);
  mat4 mat    = mat4(1.f);
  mat4 matInv = mat4(1.f);

protected:
  virtual void calcView();

  void calcOrientation(vec2 rot);
};

