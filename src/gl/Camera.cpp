#include "Camera.hpp"

#include <cassert>

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/trigonometric.hpp"
#include "../global.hpp"

Camera::Camera(vec3 pos, vec3 orientation, double sensitivity)
  : position(pos),
    orientation(orientation),
    sensitivity(sensitivity) {
  farPlane = 100.f;
  update();
};

const float& Camera::getNearPlane()     const { return nearPlane;   }
const float& Camera::getFarPlane()      const { return farPlane;    }
const float& Camera::getSpeed()         const { return speed;       }
const float& Camera::getFov()           const { return fov;         }
const vec3&  Camera::getOrientation()   const { return orientation; }
const vec3&  Camera::getPosition()      const { return position;    }
const vec3&  Camera::getUp()            const { return up;          }
const mat4&  Camera::getMatrix()        const { return mat;         }
const mat4&  Camera::getMatrixInverse() const { return matInv;      }

vec3 Camera::getBack()    const { return  transpose(view)[2]; }
vec3 Camera::getLeft()    const { return -transpose(view)[0]; }
vec3 Camera::getRight()   const { return -getLeft(); }
vec3 Camera::getForward() const { return -getBack(); }
vec3 Camera::getDown()    const { return -getUp(); }

void Camera::setNearPlane(const float& p)       { nearPlane = p; }
void Camera::setFarPlane(const float& p)        { farPlane = p; }
void Camera::setSpeedDefault(const float& s)    { speedDefult = s; speed = speedDefult * speedMul; }
void Camera::setSpeedMultiplier(const float& m) { speedMul = m; speed = speedDefult * speedMul; };

void Camera::update() {
  vec2 winSize = global::getWinSize();

  calcView();

  float aspectRatio = winSize.x / winSize.y;
  mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

  mat = proj * view;
  matInv = glm::inverse(mat);

  dvec2 winCenter = global::getWinCenter();
  if (!global::guiFocused)
    glfwSetCursorPos(global::window, winCenter.x, winCenter.y);
}

void Camera::moveForward() { position +=  orientation * speed * global::dt; }
void Camera::moveBack()    { position += -orientation * speed * global::dt; }
void Camera::moveLeft()    { position += -normalize(cross(orientation, up)) * speed * global::dt; }
void Camera::moveRight()   { position +=  normalize(cross(orientation, up)) * speed * global::dt; }

void Camera::moveUp()   { position +=  up * speed * global::dt; }
void Camera::moveDown() { position += -up * speed * global::dt; }

void Camera::moveByMouse(vec2 mousePos) {
  vec2 winSize = global::getWinSize();
  vec2 winCenter = winSize * 0.5f;
  vec2 ignore = (winCenter - mousePos) * vec2(global::guiFocused);
  vec2 pos = mousePos - winCenter + ignore;

  // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
  // and then "transforms" them into degrees
  vec2 rot = sensitivity * pos / winSize;
  vec2 radRot = glm::radians(-rot);

  calcOrientation(radRot);
}

void Camera::calcView() {
  view = lookAt(position, position + orientation, up);
}

void Camera::calcOrientation(vec2 rot) {
  float cosAngle = dot(up, orientation);
  if (cosAngle * glm::sign(rot.y) > 0.91f)
    rot.y = 0.f;

  // Rotate horizontally
  glm::quat quat = glm::angleAxis(rot.x, up);
  orientation = quat * orientation;

  // Rotate vertically
  quat = glm::angleAxis(rot.y, getRight());
  orientation = quat * orientation;
  calcView();
}

