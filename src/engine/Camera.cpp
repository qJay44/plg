#include "Camera.hpp"

#include <cassert>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "../global.hpp"

Camera::Camera(vec3 pos, float yaw, float pitch) : Moveable(pos, yaw, pitch) {
  update();
};

const float& Camera::getNearPlane()     const { return nearPlane; }
const float& Camera::getFarPlane()      const { return farPlane;  }
const float& Camera::getFov()           const { return fov;       }
const mat4&  Camera::getMatrix()        const { return mat;       }
const mat4&  Camera::getMatrixInverse() const { return matInv;    }

void Camera::setNearPlane(const float& p) { nearPlane = p; }
void Camera::setFarPlane(const float& p)  { farPlane  = p; }
void Camera::setFov(float f)              { fov       = f; }

void Camera::update() {
  vec2 winSize = global::getWinSize();

  float aspectRatio = winSize.x / winSize.y;
  mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

  view = glm::lookAt(position, position + orientation, up);
  mat = proj * view;
  matInv = glm::inverse(mat);

  dvec2 winCenter = global::getWinCenter();
  if (!global::guiFocused)
    glfwSetCursorPos(global::window, winCenter.x, winCenter.y);
}

